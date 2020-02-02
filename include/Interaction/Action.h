#pragma once
#include <string>

class Action {
	bool _editorMode;
public:
	virtual bool doAction() = 0;
	virtual ~Action() = 0;
};
