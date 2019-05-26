#pragma once
#include "nlohmann//json.hpp"

using json = nlohmann::json;

class GUISave {
private:
	std::string _fileName;
public:
	json _jsonFile;
	GUISave(std::string fileName);
	void loadFile();
	void saveToFile();
};