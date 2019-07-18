#pragma once
#include "Component.h"
#include "Common.h"
#include "Events.h"
#include "Entity.h"
#include "Texture.h"
#include "nlohmann\json.hpp"
#include "GUISave.h"

using json = nlohmann::json;

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

	void serialize(int entityID, std::shared_ptr<GUISave> save) {
	}

	void deserialize(int entityID, std::shared_ptr<GUISave> save) {
	}

	~ClickInsideComponent() {
		MouseEvent::instance().unregisterComponent(this);
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

	void serialize(int entityID, std::shared_ptr<GUISave> save) {
	}

	void deserialize(int entityID, std::shared_ptr<GUISave> save) {
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

	void serialize(int entityID, std::shared_ptr<GUISave> save) {
	}

	void deserialize(int entityID, std::shared_ptr<GUISave> save) {
	}

	~ClickClickMoveComponent() {
		MouseEvent::instance().unregisterComponent(this);
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
	std::function<void(std::shared_ptr<Entity>)> _removeFunctor;

	void initialize(InteractionMember interactionMember) {
		_interactionMember = interactionMember;
	}

	void serialize(int entityID, std::shared_ptr<GUISave> save) {
	}

	void deserialize(int entityID, std::shared_ptr<GUISave> save) {
	}

};

class InteractionCreateEntityComponent : public Component {
public:
	bool _interactReady = false;
	std::function<std::shared_ptr<Entity>(std::tuple<int, int>)> _createFunctor;
	std::function<void(int)> _removeFunctor;
	std::tuple<int, int> creationCoords;
	std::tuple<int, int> creationSize = {100, 100};

	void initialize() {
	}

	void serialize(int entityID, std::shared_ptr<GUISave> save) {
	}

	void deserialize(int entityID, std::shared_ptr<GUISave> save) {
	}


};

class TextureManagerComponent : public Component {
public:
	bool _interactReady = false;
	std::string _textureName;
	
	void initialize() {

	}

	void serialize(int entityID, std::shared_ptr<GUISave> save) {
	}

	void deserialize(int entityID, std::shared_ptr<GUISave> save) {
	}

};

class SaveLoadComponent : public Component {
public:
	bool _interactReady = false;

	void initialize() {
	}

	void serialize(int entityID, std::shared_ptr<GUISave> save) {
	}

	void deserialize(int entityID, std::shared_ptr<GUISave> save) {
	}
};