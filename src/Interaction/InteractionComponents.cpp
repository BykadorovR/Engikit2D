#include "InteractionComponents.h"

InteractionComponent::InteractionComponent() {

}

void InteractionComponent::attachOperation(std::shared_ptr<Operation> operation) {
	_interaction.push_back(operation);
}

std::tuple<float, bool> InteractionComponent::getMember(std::string name) {
	return { 0, false };
}