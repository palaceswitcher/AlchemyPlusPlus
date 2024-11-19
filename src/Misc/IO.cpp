#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "IO.hpp"

char* loadFile(const char* path) {
	/*std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr < "Error opening file\"" << path << "\"." << std::endl;
		return 1;
	}

	std::ostringstream ss;
	ss << file.rdbuf();
	return sstr.str().c_str();*/

	char* buffer;
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

    return buffer;
}
