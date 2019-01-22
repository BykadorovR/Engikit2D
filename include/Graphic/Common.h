#pragma once
#include <string>

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