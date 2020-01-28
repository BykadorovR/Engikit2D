#pragma once
#include "Component.h"
#include "Action.h"
#include "Operation.h"

class InteractionComponent : public Component {
private:
	std::vector<std::shared_ptr<Operation> > _interaction;
public:
	InteractionComponent();
	void attachOperation(std::shared_ptr<Operation> operation);
};