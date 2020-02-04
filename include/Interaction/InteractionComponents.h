#pragma once
#include "Component.h"
#include "Operation.h"

enum InteractionType {
	COMMON = 0,
	MOUSE = 1,
	KEYBOARD = 2
};

class InteractionComponent : public Component {
private:
	std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > _interaction;
public:
	InteractionComponent();
	std::tuple<float, bool> getMember(std::string name);
	void attachOperation(std::shared_ptr<Operation> operation, InteractionType type);
	std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > getOperations();
};