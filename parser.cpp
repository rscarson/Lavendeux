#include <string>
#include <iostream>
#include <regex>
#include <exception>
#include <stack>

#include "parser.h"

token nextToken(std::string* expression) {
	std::string value;
	std::cmatch match;

	// Iterate over tokens
	for(TokenIterator iterator=Tokens.begin(); iterator!=Tokens.end(); iterator++) {
		// Found a token
		if (std::regex_search(expression->c_str(), match, iterator->second)) {
			value = match[0];
			*expression = expression->substr(match[0].length());

			// Skip whipespace			
			if (iterator->first == TokenTypes::WHITESPACE)
				return nextToken(expression);

			return token{ value, iterator->first };
		}
	}
	
	// No tokens here.
	// TODO: ERROR
	std::cerr << "Warning: Bad token at '" << expression[0] << "'\n";
   	exit(0);
}

MatchLevel ruleMatch(std::stack<token> tokens, std::list<TokenTypes> rule) {
	std::stack<token> tokensCopy = tokens;
	int ruleSize = rule.size() - 1;
	bool hasMatched = false;
	std::list<TokenTypes> matchedRule;

	while (!tokensCopy.empty()) {
		if (rule.size() == 1) break;

		if (tokensCopy.top().type == rule.back()) {
			hasMatched = true;
			matchedRule.push_front(rule.back());
			rule.pop_back();
		} else if (hasMatched) {
			matchedRule.clear();
			break;
		}

		tokensCopy.pop();
	}

	if (matchedRule.size() == ruleSize) return MatchLevel::TRUE;
	else if (matchedRule.empty()) return MatchLevel::FALSE;
	else return MatchLevel::PARTIAL;
}

void reduce(std::stack<token> tokens, std::list<TokenTypes> rule) {
	std::list<token> extracted;

	if (tokens.size() < rule.size()-1) {
		// TODO: ERROR
		std::cerr << "Warning: Stack matching error!";
	   	exit(0);
	}

	while (rule.size() > 1) {
		if (rule.back() != tokens.top()) {
			// TODO: ERROR
			std::cerr << "Warning: Stack matching error!";
		   	exit(0);
		}

		extracted.push_front(tokens.top())
		tokens.pop();
		tokens.pop_back();
	}

	// TODO: Handler function for extracted list resulting value
	// TODO: Shit. Might need templates here
	token t = { "", rule.front() };
	tokens.push(t);
}

int main() {
}