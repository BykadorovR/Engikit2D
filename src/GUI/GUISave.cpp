#include "GUISave.h"
#include <fstream>
#include <iomanip>

GUISave::GUISave(std::string fileName) {
	_fileName = fileName;
}

void GUISave::loadFile() {
	std::ifstream ifs;
	ifs.open(_fileName, std::ifstream::in);
	if (ifs.is_open()) {
		_jsonFile = json::parse(ifs);
	}
}

void GUISave::saveToFile() {
	std::ofstream file(_fileName);
	file << std::setw(4) << _jsonFile << std::endl;
}