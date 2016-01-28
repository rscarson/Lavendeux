#ifndef PARSER_H
	#define PARSER_H
	#include <string>
	#include <list>
	#include <map>
	#include <regex>

	// Token definitions
	enum TokenTypes {
		ID, INT, FLOAT, HEX, SCI, // Value types
		ADD, SUB, MUL, DIV, MOD, // Math operators
		NOT, AND, OR, XOR, LSHIFT, RSHIFT, // Boolean operators
		EQUAL, LPAREN, RPAREN, WHITESPACE, // Syntactical tokens

		// Gramatical tokens
		value, expression, not_expression, boolean_expression, 
		multiplicative_expression, additive_expression, assignment_expression, 
		complete
	};
	typedef std::map<TokenTypes, std::regex>::iterator TokenIterator;
	static std::map<TokenTypes, std::regex> Tokens = {
		// Values
		{TokenTypes::ID, std::regex("^[a-zA-Z_][a-zA-Z_0-9]*")},
		{TokenTypes::INT, std::regex("^-{0,1}[0-9]+")},
		{TokenTypes::FLOAT, std::regex("^-{0,1}[0-9]*\\.[0-9]+")},
		{TokenTypes::HEX, std::regex("^0x[0-9a-fA-F]+")},
		{TokenTypes::SCI, std::regex("^-{0,1}[0-9]*(\\.[0-9]+){0,1}E-{0,1}[0-9]+")},

		// Math 
		{TokenTypes::ADD, std::regex("^\\+")},
		{TokenTypes::SUB, std::regex("^-")},
		{TokenTypes::MUL, std::regex("^\\*")},
		{TokenTypes::DIV, std::regex("^/")},
		{TokenTypes::MOD, std::regex("^\%")},

		// Boolean 
		{TokenTypes::NOT, std::regex("^!")},
		{TokenTypes::AND, std::regex("^&")},
		{TokenTypes::OR, std::regex("^\\|")},
		{TokenTypes::XOR, std::regex("^\\^")},
		{TokenTypes::LSHIFT, std::regex("^<")},
		{TokenTypes::RSHIFT, std::regex("^>")},

		// Syntactical 
		{TokenTypes::EQUAL, std::regex("^\\=")},
		{TokenTypes::LPAREN, std::regex("^\\(")},
		{TokenTypes::RPAREN, std::regex("^\\)")},
		{TokenTypes::WHITESPACE, std::regex("^\\s+")},
	};
	
	typedef struct {
		std::string value;
		TokenTypes type;
	} token;

	token nextToken(std::string expression);

	// Grammar rules (first vector element is result)
	enum MatchLevel { FALSE, PARTIAL, TRUE };
	static std::vector< std::list<TokenTypes> > Grammar = {
		std::list<TokenTypes>({TokenTypes::value, TokenTypes::ID}),
		std::list<TokenTypes>({TokenTypes::value, TokenTypes::INT}),
		std::list<TokenTypes>({TokenTypes::value, TokenTypes::FLOAT}),
		std::list<TokenTypes>({TokenTypes::value, TokenTypes::HEX}),
		std::list<TokenTypes>({TokenTypes::value, TokenTypes::SCI}),

		std::list<TokenTypes>({TokenTypes::expression, TokenTypes::value}),
		std::list<TokenTypes>({TokenTypes::expression, TokenTypes::LPAREN, TokenTypes::expression, TokenTypes::RPAREN}),
		
		std::list<TokenTypes>({TokenTypes::not_expression, TokenTypes::expression}),
		std::list<TokenTypes>({TokenTypes::not_expression, TokenTypes::NOT, TokenTypes::not_expression}),
		
		std::list<TokenTypes>({TokenTypes::boolean_expression, TokenTypes::not_expression}),
		std::list<TokenTypes>({TokenTypes::boolean_expression, TokenTypes::boolean_expression, TokenTypes::AND, TokenTypes::boolean_expression}),
		std::list<TokenTypes>({TokenTypes::boolean_expression, TokenTypes::boolean_expression, TokenTypes::OR, TokenTypes::boolean_expression}),
		std::list<TokenTypes>({TokenTypes::boolean_expression, TokenTypes::boolean_expression, TokenTypes::XOR, TokenTypes::boolean_expression}),
		std::list<TokenTypes>({TokenTypes::boolean_expression, TokenTypes::boolean_expression, TokenTypes::LSHIFT, TokenTypes::boolean_expression}),
		std::list<TokenTypes>({TokenTypes::boolean_expression, TokenTypes::boolean_expression, TokenTypes::RSHIFT, TokenTypes::boolean_expression}),
		
		std::list<TokenTypes>({TokenTypes::multiplicative_expression, TokenTypes::boolean_expression}),
		std::list<TokenTypes>({TokenTypes::multiplicative_expression, TokenTypes::multiplicative_expression, TokenTypes::MUL, TokenTypes::multiplicative_expression}),
		std::list<TokenTypes>({TokenTypes::multiplicative_expression, TokenTypes::multiplicative_expression, TokenTypes::MOD, TokenTypes::multiplicative_expression}),
		std::list<TokenTypes>({TokenTypes::multiplicative_expression, TokenTypes::multiplicative_expression, TokenTypes::DIV, TokenTypes::multiplicative_expression}),
		
		std::list<TokenTypes>({TokenTypes::additive_expression, TokenTypes::multiplicative_expression}),
		std::list<TokenTypes>({TokenTypes::additive_expression, TokenTypes::additive_expression, TokenTypes::ADD, TokenTypes::additive_expression}),
		std::list<TokenTypes>({TokenTypes::additive_expression, TokenTypes::additive_expression, TokenTypes::SUB, TokenTypes::additive_expression}),
		
		std::list<TokenTypes>({TokenTypes::assignment_expression, TokenTypes::additive_expression}),
		std::list<TokenTypes>({TokenTypes::assignment_expression, TokenTypes::ID, TokenTypes::EQUAL, TokenTypes::assignment_expression}),
		
		std::list<TokenTypes>({TokenTypes::complete, TokenTypes::assignment_expression}),
	};

	MatchLevel ruleMatch(std::stack<token> tokens, std::list<TokenTypes> rule);
	std::list<token> reduce(std::stack<token> tokens, std::list<TokenTypes> rule);
#endif