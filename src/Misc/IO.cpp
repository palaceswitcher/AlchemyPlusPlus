#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "IO.hpp"

std::string loadFile(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "ERROR: File \"" << path << "\" not found.\n";
		return ""; //Return nothing if the file doesn't exist
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();

	/*char* buffer;
	long length;
	FILE* f = fopen (path, "rb"); //Open file
	if (f == NULL) {
		perror("Error loading file.");
	}

	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f); //Get file size
		fseek(f, 0, SEEK_SET); //Move back to start of file
		buffer = (char*) malloc((length+1)*sizeof(char)); //Allocate space for file
		if (buffer) {
			if (fread(buffer, sizeof(char), length, f) != length) {
				perror("Error reading file.");
			}
		}
		fclose (f);
	}
	buffer[length] = '\0'; //Null terminate buffer

    return buffer;*/
}
