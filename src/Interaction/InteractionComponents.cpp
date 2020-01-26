#include "InteractionComponents.h"

InteractionComponent::InteractionComponent() {

}

void InteractionComponent::attachAction(std::shared_ptr<Operation> operation, std::shared_ptr<Action> action) {
	_interaction[operation].push_back(action);
}