#include "IO.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::string loadFile(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "ERROR: File \"" << path << "\" not found.\n";
		return ""; // Return nothing if the file doesn't exist
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
