#include "InteractionComponents.h"

InteractionComponent::InteractionComponent() {
	_componentName = "InteractionComponent";
}

void InteractionComponent::attachOperation(std::shared_ptr<Operation> operation, InteractionType type) {
	_interaction.push_back({ operation, type });
}

std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > InteractionComponent::getOperations() {
	return _interaction;
}

std::tuple<float, bool> InteractionComponent::getMember(std::string name) {
	return { 0, false };
}