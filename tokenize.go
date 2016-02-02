package main

import (
	"regexp"
)

func staticCompile(pattern string) *regexp.Regexp {
	p, err := regexp.Compile(pattern)
	if err != nil {
		panic(err)
	}

	return p
}

type TokenType struct {
	name string
	regex *regexp.Regexp
}

var Tokens = []TokenType {
	{ "ID", staticCompile("^[[:alpha:]][[:alnum:]]*") },
	{ "HEX", staticCompile("^0x[0-9a-fA-F]+") },
	{ "BIN", staticCompile("^0b[0-1]+") },
	{ "OCT", staticCompile("^0o[0-7]+") },
	{ "SCI", staticCompile("^-?[[:digit:]]*(\\.[[:digit:]]+)?(E|e)(\\+|-)?[[:digit:]]+") },
	{ "FLOAT", staticCompile("^-?[[:digit:]]*\\.[[:digit:]]+") },
	{ "INT", staticCompile("^-?[[:digit:]]+") },

	{ "ADD", staticCompile("^\\+") },
	{ "SUB", staticCompile("^-") },
	{ "MUL", staticCompile("^\\*") },
	{ "DIV", staticCompile("^\\/") },
	{ "MOD", staticCompile("^\\%") },

	{ "NOT", staticCompile("^!") },
	{ "AND", staticCompile("^&") },
	{ "OR", staticCompile("^\\|") },
	{ "XOR", staticCompile("^\\^") },
	{ "LSHIFT", staticCompile("^<") },
	{ "RSHIFT", staticCompile("^>") },

	{ "EQUAL", staticCompile("^=") },
	{ "LPAREN", staticCompile("^\\(") },
	{ "RPAREN", staticCompile("^\\)") },
	{ "WHITESPACE", staticCompile("^[[:space:]]+") },
}

type Token struct {
	v string
	t string

	val TokenValue
}

func nextToken(expression *string) Token {
	for i := range Tokens {
		var token = Tokens[i]
		match := token.regex.FindString(*expression)
		*expression = (*expression)[len(match):]
		if len(match) > 0 {
			if token.name == "WHITESPACE" { return nextToken(expression) }

			return Token { match, token.name, TokenValue{} }
		}
	}

	panic("Invalid token in expression!")
}