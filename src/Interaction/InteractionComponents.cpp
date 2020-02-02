#include "InteractionComponents.h"

InteractionComponent::InteractionComponent() {

}

void InteractionComponent::attachOperation(std::shared_ptr<Operation> operation) {
	_interaction.push_back(operation);
}

std::vector<std::shared_ptr<Operation> > InteractionComponent::getOperations() {
	return _interaction;
}

std::tuple<float, bool> InteractionComponent::getMember(std::string name) {
	return { 0, false };
}