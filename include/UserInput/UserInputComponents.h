#pragma once
#include "Component.h"

class MouseComponent : public Component {
public:
	MouseComponent();
private:
	std::tuple<float, float> _leftClick;
	std::tuple<float, float> _rightClick;
};