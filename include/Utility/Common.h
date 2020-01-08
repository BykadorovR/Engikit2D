#pragma once
#include <iostream>
#include <sstream>
#include "Windows.h"
#include <string>

std::string rawFileContent(const char* relativePath);

#define OUT_STREAM( s ) \
{ \
   std::ostringstream os_; \
   os_ << s; \
   OutputDebugString( os_.str().c_str() ); \
}