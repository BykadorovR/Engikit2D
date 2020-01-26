#pragma once
#include <string>

class Action {
	bool _editorMode;
public:
	virtual ~Action() = 0;
};

class ActionTest : public Action {
public:
	ActionTest();
};