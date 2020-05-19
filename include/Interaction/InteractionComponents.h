#pragma once
#include "Component.h"
#include "Operation.h"

enum InteractionType {
	COMMON_START = 0,
	COMMON_END,
	MOUSE_START,
	MOUSE_END,
	KEYBOARD_START,
	KEYBOARD_END
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