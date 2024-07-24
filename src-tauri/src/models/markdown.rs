use std::{collections::HashMap, iter::Peekable, slice::Iter};

use serde::{Deserialize, Serialize};

#[derive(Debug, PartialEq, Clone, Serialize, Deserialize)]
enum MarkdownToken {
    H4(String),
    H3(String),
    H2(String),
    H1(String),
    HR,
    CodeBlock(String, String),
    UL(Vec<String>),
    PlainText(String),
}

impl MarkdownToken {
    pub fn tokenize(input: &str) -> Vec<Self> {
        let mut tokens = Vec::default();

        let mut lines = input.lines();
        while let Some(line) = lines.next() {
            let mut line = line.trim();

            if line.starts_with('#') {
                // Heading
                let mut l_lvl = 0;
                while line.starts_with('#') {
                    l_lvl += 1;
                    // remove the first character
                    line = &line[1..];
                }

                match l_lvl {
                    1 => tokens.push(Self::H1(line.trim().to_string())),
                    2 => tokens.push(Self::H2(line.trim().to_string())),
                    3 => tokens.push(Self::H3(line.trim().to_string())),
                    4 => tokens.push(Self::H4(line.trim().to_string())),
                    _ => tokens.push(Self::PlainText(line.trim().to_string())),
                }
            } else if line.starts_with("---") {
                // Heading horizontal break
                tokens.push(Self::HR)
            } else if line.starts_with("```") {
                // Codeblock
                let syntax = line.replace("```", "").trim().to_string();
                let mut buffer = String::default();
                for line in lines.by_ref() {
                    if line.starts_with("```") {
                        break;
                    }
                    buffer += &format!("{line}\n");
                }
                tokens.push(Self::CodeBlock(syntax, buffer.trim().to_string()));
            } else if line.starts_with('-') {
                // Unordered list
                let mut ul_lines = vec![line.replace('-', "").trim().to_string()];
                for line in lines.by_ref() {
                    if line.starts_with('-') {
                        ul_lines.push(line.replace('-', "").trim().to_string());
                    } else {
                        break;
                    }
                }
                tokens.push(Self::UL(ul_lines));
            } else if !line.trim().is_empty() {
                // Plain text
                tokens.push(Self::PlainText(line.to_string()));
            }
        }

        tokens
    }
}

#[derive(Debug, PartialEq, Clone, Serialize, Deserialize)]
pub struct MarkdownTree {
    heading: String,
    contents: Vec<MarkdownToken>,
    subheadings: HashMap<String, Vec<MarkdownToken>>,
}

impl MarkdownTree {
    pub fn parse(input: &str) -> Self {
        let mut root = Self {
            heading: String::default(),
            contents: Vec::default(),
            subheadings: HashMap::default(),
        };
        let tokens = MarkdownToken::tokenize(input);
        let mut token_iter = tokens.iter().peekable();
        loop {
            if let Some(token) = token_iter.next() {
                match token {
                    MarkdownToken::H1(_) => continue,
                    MarkdownToken::H2(s) => {
                        root.subheadings
                            .insert(s.to_string(), Self::parse_subtree(&mut token_iter));
                    }
                    _ => root.contents.push(token.clone()),
                }
            } else {
                return root;
            }
        }
    }

    fn parse_subtree(token_iter: &mut Peekable<Iter<'_, MarkdownToken>>) -> Vec<MarkdownToken> {
        let mut contents = Vec::default();
        loop {
            if token_iter.peek().is_some()
                && matches!(token_iter.peek().unwrap(), MarkdownToken::H2(_))
            {
                return contents;
            }
            if let Some(token) = token_iter.next() {
                match token {
                    MarkdownToken::H1(_) => continue,
                    _ => contents.push(token.clone()),
                }
            } else {
                return contents;
            }
        }
    }
}

#[cfg(test)]
mod test_parse {
    use super::*;

    static TEST_FILE: &str = include_str!("../../../language/help/en.help.md");

    #[test]
    fn test_lex() {
        let tokens = MarkdownToken::tokenize(TEST_FILE);
        println!("{:?}", tokens);
    }

    #[test]
    fn test_parse() {
        let tree = MarkdownTree::parse(TEST_FILE);
        println!("{:?}", tree);
    }
}
