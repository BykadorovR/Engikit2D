#pragma once
#include <string>

class Action {
protected:
	bool _editorMode;
	std::string _actionName;
public:
	Action();
	virtual bool doAction();
	virtual ~Action() = 0;
};
