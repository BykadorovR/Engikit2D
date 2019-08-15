#pragma once
#include <string>
#include <utility>
#include <map>
#include <iostream>
#include "World.h"
#include <sstream>
#include "Windows.h"
#include "Entity.h"

const std::pair<int, int> resolution(1920, 1080);
static World world;
static Entity currentSprite;

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

#define OUT_STREAM( s ) \
{ \
   std::ostringstream os_; \
   os_ << s; \
   OutputDebugString( os_.str().c_str() ); \
}

std::string rawFileContent(const char* relativePath);

const int POSITION_COMPONENT_COUNT = 2;
const int BYTES_PER_FLOAT = 4;
const int TEXTURE_COORDINATES_COMPONENT_COUNT = 2;
const int STRIDE = (POSITION_COMPONENT_COUNT + TEXTURE_COORDINATES_COMPONENT_COUNT) * BYTES_PER_FLOAT;