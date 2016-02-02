package main

import (
	"math"
	"strconv"
)

var AllocatedVariables = map[string] TokenValue {
	"pi": TokenValue{ FloatValue, 0, math.Pi },
	"phi": TokenValue{ FloatValue, 0, math.Phi },
	"e": TokenValue{ FloatValue, 0, math.E },
}

type TokenValueType int
const (
	NoValue TokenValueType = iota
	IntValue TokenValueType = iota
	FloatValue TokenValueType = iota
)

type TokenValue struct {
	t TokenValueType

	iv int64
	fv float64
}

// Stack contains one token
func value_handler(tokens []Token) {
	switch (tokens[0].t) {
	case "ID":

	case "INT":
		value, err := strconv.ParseInt(tokens[0].v, 0, 64)
		if err != nil {
			return TokenValue{ IntValue, value, 0 }
		}
	case "FLOAT":
		value, err := strconv.ParseFloat(tokens[0].v, 64)
		if err != nil {
			return TokenValue{ FloatValue, 0, value }
		}
	case "HEX":

	case "SCI":

	}

	panic("Value error")
}

/* Grammar
	value
		ID
		INT
		FLOAT
		HEX
		SCI

	function


	expression
		value
		LPAREN expression RPAREN
			
	not_expression
		expression
		NOT not_expression
			
	boolean_expression
		not_expression
		boolean_expression AND boolean_expression
		boolean_expression OR boolean_expression
		boolean_expression XOR boolean_expression
		boolean_expression LSHIFT boolean_expression
		boolean_expression RSHIFT boolean_expression
			
	multiplicative_expression
		boolean_expression
		multiplicative_expression MUL multiplicative_expression
		multiplicative_expression MOD multiplicative_expression
		multiplicative_expression DIV multiplicative_expression
			
	additive_expression
		multiplicative_expression
		additive_expression ADD additive_expression
		additive_expression SUB additive_expression
			
	assignment_expression
		additive_expression
		ID EQUAL assignment_expression
			
	complete
		assignment_expression
*/