#pragma once
#include <string>

class Action {
	bool _editorMode;
public:
	virtual ~Action() = 0;
};

class ChangeNumberAction : public Action {
public:
	ActionTest();
};