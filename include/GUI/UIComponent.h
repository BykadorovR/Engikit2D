#pragma once
#include "Component.h"
#include "Common.h"
#include "Window.h"
#include "Events.h"

class ClickMoveComponent : public Component, IMouseEvent {
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

	~ClickMoveComponent() {
		MouseEvent::instance().unregisterComponent(this);
	}
};

class ClickInsideComponent : public Component, IMouseEvent {
public:
	std::tuple<int, int> _leftClick;
	std::tuple<int, int> _rightClick;
	bool _rightClickFlag = false;
	bool _leftClickFlag = false;
	bool _moveToByClick = false;

	void initialize(bool moveToByClick = false) {
		MouseEvent::instance().registerComponent(this);
		_moveToByClick = moveToByClick;
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

class ClickClickMoveComponent : public Component, IMouseEvent {
public:
	std::tuple<int, int> _currentClick;
	std::tuple<int, int> _previousClick;
	bool _clickFlag = false;
	bool _moveToByClickFirst = false;
	bool _moveToByClickSecond = false;


	void initialize(bool moveToByClickFirst = false, bool moveToByClickSecond = false) {
		MouseEvent::instance().registerComponent(this);
		_moveToByClickFirst = moveToByClickFirst;
		_moveToByClickSecond = moveToByClickSecond;
	}

	void mouseClickDownLeft(int x, int y) {
		_currentClick = { x, y };
		_clickFlag = true;
	}

	void mouseClickDownRight(int x, int y) {
	}
};

enum InteractionMember {
	OBJECT = 0,
	SUBJECT = 1
};


class InteractionAddToEntityComponent : public Component {
public:
	InteractionMember _interactionMember;
	bool _interactReady = false;
	std::function<std::shared_ptr<Component>(void)> _createFunctor;

	void initialize(InteractionMember interactionMember) {
		_interactionMember = interactionMember;
	}

};