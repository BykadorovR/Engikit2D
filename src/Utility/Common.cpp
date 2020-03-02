#include "Common.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "assert.h"
#include <cctype>
#include "Windows.h"
#include <locale>

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

bool isNumber(std::string s) {
	std::size_t char_pos(0);
	std::locale loc;
	if (s[char_pos] == '+' || s[char_pos] == '-') ++char_pos; // skip the sign if exist
	int nunmberDigits, numberPoints;
	for (nunmberDigits = 0, numberPoints = 0; std::isdigit(s[char_pos], loc) || s[char_pos] == '.'; ++char_pos) {
		s[char_pos] == '.' ? ++numberPoints : ++nunmberDigits;
	}
	if (numberPoints > 1 || nunmberDigits < 1) // no more than one point, at least one digit
		return false;

	return char_pos == s.size();  // must reach the ending 0 of the string
}

std::string convertMultibyteToUTF8(std::string input) {
	int sizeMultibyte = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, input.c_str(),
		input.length(), nullptr, 0);

	std::wstring utf16String(sizeMultibyte, '\0');
	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, input.c_str(),
		input.length(), &utf16String[0], sizeMultibyte);

	int utf8Size = WideCharToMultiByte(CP_UTF8, 0, utf16String.c_str(),
		utf16String.length(), nullptr, 0,
		nullptr, nullptr);
	std::string utf8String(utf8Size, '\0');
	WideCharToMultiByte(CP_UTF8, 0, utf16String.c_str(),
		utf16String.length(), &utf8String[0], utf8Size,
		nullptr, nullptr);
	return utf8String;
}