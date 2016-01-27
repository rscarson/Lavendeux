#include <string>
#include <iostream>
#include <regex>
#include <exception>

#include "parser.h"

token nextToken(std::string expression) {}

int main() {
	try {
		std::regex("\\|", std::regex_constants::extended);
	} catch (std::regex_error& e) {
     std::cerr << e.code();
	}
}