#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#define NOMINMAX
#include "Windows.h"

std::string rawFileContent(const char* relativePath);

#define OUT_STREAM( s ) \
{ \
   std::ostringstream os_; \
   os_ << s; \
   OutputDebugString( os_.str().c_str() ); \
}

bool isNumber(std::string s);

std::string convertMultibyteToUTF8(std::string input);
