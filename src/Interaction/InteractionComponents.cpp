#include "InteractionComponents.h"
#include <algorithm>

InteractionComponent::InteractionComponent() {
	_componentName = "InteractionComponent";
}

bool InteractionComponent::attachOperation(std::shared_ptr<Operation> operation, InteractionType type) {
	_interaction.push_back({ operation, type });
	return false;
}

std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > InteractionComponent::getOperations() {
	return _interaction;
}

std::tuple<float, bool> InteractionComponent::getMember(std::string name) {
	return { 0, false };
}

bool InteractionComponent::clearOperations(std::string name) {
	for (auto it = _interaction.begin(); it != _interaction.end();) {
		if (std::get<0>(*it)->getName() == name)
			it = _interaction.erase(it);
		else
			++it;
	}
	
	return false;
}