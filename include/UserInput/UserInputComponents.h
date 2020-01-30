#pragma once
#include "Component.h"
#include "UserInputEvents.h"

class MouseComponent : public Component, public IMouseEvent {
public:
	MouseComponent();
	void mouseClickDownLeft(int x, int y);
	void mouseClickDownRight(int x, int y);
	~MouseComponent();
private:
	std::tuple<float, float> _leftClick;
	std::tuple<float, float> _rightClick;
};