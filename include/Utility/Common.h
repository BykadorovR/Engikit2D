#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#ifdef WIN32
#define NOMINMAX
#include "Windows.h"
#endif

std::string rawFileContent(const char* relativePath);

//TODO: make some more smart and portable solution for writing logs
#ifdef WIN32
#define OUT_STREAM( s ) \
{ \
   std::ostringstream os_; \
   os_ << s; \
   OutputDebugString( os_.str().c_str() ); \
}

std::string convertMultibyteToUTF8(std::string input);

#endif

bool isNumber(std::string s);