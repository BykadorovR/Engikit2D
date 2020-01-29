#pragma once
#include "Component.h"
#include "UserInputEvents.h"

class MouseComponent : public Component, public IMouseEvent {
public:
	MouseComponent();
	float getMember(std::string name);
	void mouseClickDownLeft(int x, int y);
	void mouseClickDownRight(int x, int y);
	~MouseComponent();
private:
	std::map<std::string, float*> _classVariables;
	std::tuple<float, float> _leftClick;
	std::tuple<float, float> _rightClick;
};