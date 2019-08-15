#include "Common.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "assert.h"

World world;

std::string rawFileContent(const char* relativePath) {
	std::string content;
	std::ifstream fileStream(relativePath, std::ios::binary);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << relativePath << ". File does not exist." << std::endl;
		assert(false);
	}

	std::stringstream sstr;
	while (fileStream >> sstr.rdbuf());
	content = sstr.str();
	fileStream.close();
	return content;
}