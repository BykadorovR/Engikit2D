#pragma once
#include "Component.h"
#include "Operation.h"

enum InteractionType {
	COMMON_START = 0,
	COMMON_END = 1,
	MOUSE_START = 2,
	MOUSE_END = 3,
	KEYBOARD_START = 4,
	KEYBOARD_END = 5
};

class InteractionComponent : public Component {
private:
	std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > _interaction;
public:
	InteractionComponent();
	std::tuple<float, bool> getMember(std::string name);
	bool attachOperation(std::shared_ptr<Operation> operation, InteractionType type);
	std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > getOperations();
	bool clearOperations(std::string name);
};