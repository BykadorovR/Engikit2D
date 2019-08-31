#include "Common.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "assert.h"

World world;
std::pair<int, int> resolution(1920, 1080);
float resolutionRatioX = (float) resolution.first / (float) 1920;
float resolutionRatioY = (float) resolution.second / (float) 1080;


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