use std::collections::HashMap;

use logos::{Lexer, Logos};

fn code_block(lex: &mut Lexer<MarkdownToken>) -> String {
    let lines = lex.to_string().split("\n");
    let syntax = lines[0].replace("```", "").trim();
    let code = lex
        .to_string()
        .replace(lines[0], "")
        .replace("```", "")
        .trim();
    code
}

#[derive(Logos, Debug, PartialEq)]
enum MarkdownToken {
    #[regex("\\W*##\\W*[^\n]+", |lex| lex.to_string().replace("#", "").trim())]
    H2(String),

    #[regex("\\W*#\\W*[^\n]+", |lex| lex.to_string().replace("#", "").trim())]
    H1(String),

    #[regex("\\W*-{3,}\n")]
    HR,

    #[regex("\\W*```[a-zA-Z0-9]+\n(.*\n)*\\W*```", code_block)]
    CodeBlock(String),

    #[regex(".+")]
    PlainText(String),
}

struct MarkdownTree {
    heading: String,
    contents: Vec<MarkdownToken>,
    subheadings: HashMap<String, MarkdownTree>,
}

fn tokenize_md(input: String) -> Vec<MarkdownToken> {
    MarkdownToken::lexer(input).collect()
}

#[cfg(test)]
mod test_parse {
    use super::*;

    #[test]
    fn test_lex() {
        let tokens = tokenize_md(include_str!("../../../language/help/en.help.md").to_string());
        println!("{:?}", tokens);
    }
}
