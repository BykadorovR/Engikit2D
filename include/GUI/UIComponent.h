#pragma once
#include "Component.h"
#include "Common.h"
#include "Window.h"
#include "Events.h"

class PointMoveComponent : public Component, IMouseEvent {
public:
	std::tuple<int, int> _leftClick;
	std::tuple<int, int> _rightClick;
	float _speed;
	bool _move = false;

	void initialize(float speed) {
		MouseEvent::instance().registerComponent(this);
		_speed = speed;
	}

	void mouseClickDownLeft(int x, int y) {
		_leftClick = { x, y };
	}
	void mouseClickDownRight(int x, int y) {
		_rightClick = { x, y };
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
		_leftClick = { x, y };
		_leftClickFlag = true;
	}
	void mouseClickDownRight(int x, int y) {
		_rightClick = { x, y };
		_rightClickFlag = true;
	}
};

class GroupEntitiesComponent : public Component {
public:
	uint32_t _groupNumber;
	std::string _groupName;

	void initialize(uint32_t groupNumber, std::string groupName) {
		_groupNumber = groupNumber;
		_groupName = groupName;
	}
};

class ClickToMoveComponent : public Component, IMouseEvent {
public:
	std::tuple<int, int> _currentClick;
	std::tuple<int, int> _previousClick;
	bool _clickFlag = false;

	void initialize() {
		MouseEvent::instance().registerComponent(this);
	}

	void mouseClickDownLeft(int x, int y) {
		_currentClick = { x, y };
		_clickFlag = true;
	}

	void mouseClickDownRight(int x, int y) {
	}
};