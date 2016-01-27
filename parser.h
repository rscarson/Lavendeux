#ifndef PARSER_H
	#define PARSER_H
	#include <string>
	#include <map>
	#include <regex>

	typedef struct {
		std::string value;
		char type;
	} token;

	token nextToken(std::string expression);

	// Token definitions
	enum TokenTypes {
		ID, INT, FLOAT, HEX, SCI, // Value types
		ADD, SUB, MUL, DIV, MOD, // Math operators
		NOT, AND, OR, XOR, LSHIFT, RSHIFT, // Boolean operators
		LPAREN, RPAREN // Syntactical tokens
	};
	static std::map<TokenTypes, std::regex> Tokens = {
		// Values
		{TokenTypes::ID, std::regex("[a-zA-Z_][a-zA-Z_0-9]*", std::regex::extended)},
		{TokenTypes::INT, std::regex("-{0,1}[0-9]+", std::regex::extended)},
		{TokenTypes::FLOAT, std::regex("-{0,1}[0-9]*\\.[0-9]+", std::regex::extended)},
		{TokenTypes::HEX, std::regex("0x[0-9a-fA-F]+", std::regex::extended)},
		{TokenTypes::SCI, std::regex("-{0,1}[0-9]*(\\.[0-9]+){0,1}E-{0,1}[0-9]+", std::regex::extended)},

		// Math 
		{TokenTypes::ADD, std::regex("+", std::regex::extended)},
		{TokenTypes::SUB, std::regex("-", std::regex::extended)},
		{TokenTypes::MUL, std::regex("\\*", std::regex::extended)},
		{TokenTypes::DIV, std::regex("/", std::regex::extended)},
		{TokenTypes::MOD, std::regex("\%", std::regex::extended)},

		// Boolean 
		{TokenTypes::NOT, std::regex("!", std::regex::extended)},
		{TokenTypes::AND, std::regex("&", std::regex::extended)},
	//	{TokenTypes::OR, std::regex("\\|", std::regex::extended)},
		{TokenTypes::XOR, std::regex("\\^", std::regex::extended)},
		{TokenTypes::LSHIFT, std::regex("<", std::regex::extended)},
		{TokenTypes::RSHIFT, std::regex(">", std::regex::extended)},

		// Syntactical 
		{TokenTypes::LPAREN, std::regex("\\(", std::regex::extended)},
		{TokenTypes::RPAREN, std::regex("\\)", std::regex::extended)}
	};
#endif