#include "Common.h"
#include <fstream>
#include <iostream>
#include "assert.h"

std::string rawFileContent(const char* relativePath) {
	std::string content;
	std::ifstream fileStream(relativePath, std::ios::binary);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << relativePath << ". File does not exist." << std::endl;
		assert(false);
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}