#pragma once
#include <string>
#include <utility>
const std::pair<int, int> resolution(720, 480);

enum ErrorCodes {
	TW_OK = 0,
	TW_ERROR = 1
};

#define CHECK_STATUS(status) \
	if (status != 0) { \
		std::cout << "Error status != 0\n" << std::flush; \
		std::cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << "\n" << std::flush; \
		return status; \
	} \

std::string rawFileContent(const char* relativePath);

const int POSITION_COMPONENT_COUNT = 2;
const int BYTES_PER_FLOAT = 4;
const int TEXTURE_COORDINATES_COMPONENT_COUNT = 2;
const int STRIDE = (POSITION_COMPONENT_COUNT + TEXTURE_COORDINATES_COMPONENT_COUNT) * BYTES_PER_FLOAT;