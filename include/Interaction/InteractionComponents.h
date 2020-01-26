#pragma once
#include "Component.h"
#include "Action.h"
#include "Operation.h"

class InteractionComponent : public Component {
private:
	std::map<std::shared_ptr<Operation>, std::vector<std::shared_ptr<Action> > > _interaction;
public:
	InteractionComponent();
	void attachAction(std::shared_ptr<Operation> operation, std::shared_ptr<Action> action);
};