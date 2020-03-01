#pragma once
#include "Component.h"

class MouseComponent : public Component {
public:
	MouseComponent();
private:
	std::tuple<float, float> _leftClick;
	std::tuple<float, float> _rightClick;
};

class KeyboardComponent : public Component {
public:
	KeyboardComponent();
private:
	std::string _symbol = "";
};