#include "InteractionComponents.h"

InteractionComponent::InteractionComponent() {

}

void InteractionComponent::attachOperation(std::shared_ptr<Operation> operation) {
	_interaction.push_back(operation);
}