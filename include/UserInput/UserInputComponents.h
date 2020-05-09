#pragma once
#include "Component.h"

class MouseComponent : public OperationComponent {
public:
	MouseComponent();
private:
	std::tuple<float, float> _leftClick;
	std::tuple<float, float> _rightClick;
};

class KeyboardComponent : public OperationComponent {
public:
	KeyboardComponent();
private:
	std::string _symbol = "";
	float _code;
};