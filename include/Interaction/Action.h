#pragma once
#include <string>

class Action {
	bool _editorMode;
public:
	Action();
	bool compare();
	bool prepare(std::string config);
};