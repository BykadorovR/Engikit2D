#pragma once
#include "Component.h"
#include "Operation.h"

class InteractionComponent : public Component {
private:
	std::vector<std::shared_ptr<Operation> > _interaction;
public:
	InteractionComponent();
	std::tuple<float, bool> getMember(std::string name);
	void attachOperation(std::shared_ptr<Operation> operation);
};