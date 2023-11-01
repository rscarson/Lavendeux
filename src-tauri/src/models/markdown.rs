use std::{collections::HashMap, iter::Peekable, slice::Iter};

use serde::{Deserialize, Serialize};

#[derive(Debug, PartialEq, Clone, Serialize, Deserialize)]
enum MarkdownToken {
    H2(String),
    H1(String),
    HR,
    CodeBlock(String, String),
    PlainText(String),
}

impl MarkdownToken {
    pub fn tokenize(input: &str) -> Vec<Self> {
        let mut tokens = Vec::default();

        let mut code_syntax_buf: Option<String> = None;
        let mut buffer = String::default();

        for line in input.lines() {
            let line = line.trim();

            if line.starts_with("##") {
                // Heading Lvl2
                tokens.push(Self::H2(line.replace('#', "").trim().to_string()))
            } else if line.starts_with("#") {
                // Heading Lvl1
                tokens.push(Self::H1(line.replace('#', "").trim().to_string()))
            } else if line.starts_with("---") {
                // Heading horizontal break
                tokens.push(Self::HR)
            } else if line.starts_with("```") {
                if code_syntax_buf.is_none() {
                    code_syntax_buf = Some(line.replace("```", "").trim().to_string());
                } else {
                    tokens.push(Self::CodeBlock(
                        code_syntax_buf.unwrap(),
                        buffer.trim().to_string(),
                    ));
                    code_syntax_buf = None;
                    buffer = String::default();
                }
            } else {
                if code_syntax_buf.is_none() {
                    if !line.trim().is_empty() {
                        tokens.push(Self::PlainText(line.to_string()));
                    }
                } else {
                    buffer += &format!("{line}\n");
                }
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

    static TEST_FILE: &'static str = include_str!("../../../language/help/en.help.md");

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
