#pragma once
#include "Component.h"
#include "Common.h"
#include "Window.h"
#include "Events.h"

class PointMoveComponent : public Component, IMouseEvent {
public:
	std::tuple<int, int> _leftClick;
	std::tuple<int, int> _rightClick;

	void initialize() {
		MouseEvent::instance().registerComponent(this);
	}

	void mouseClickDownLeft(int x, int y) {
		std::get<0>(_leftClick) = x;
		std::get<1>(_leftClick) = y;

	}
	void mouseClickDownRight(int x, int y) {
		std::get<0>(_rightClick) = x;
		std::get<1>(_rightClick) = y;
	}
};

class ClickInsideComponent : public Component, IMouseEvent {
public:
	std::tuple<int, int> _leftClick;
	std::tuple<int, int> _rightClick;
	bool _rightClickFlag = false;
	bool _leftClickFlag = false;

	void initialize() {
		MouseEvent::instance().registerComponent(this);
	}

	void mouseClickDownLeft(int x, int y) {
		std::get<0>(_leftClick) = x;
		std::get<1>(_leftClick) = y;
		_leftClickFlag = true;
	}
	void mouseClickDownRight(int x, int y) {
		std::get<0>(_rightClick) = x;
		std::get<1>(_rightClick) = y;
		_rightClickFlag = true;
	}
};