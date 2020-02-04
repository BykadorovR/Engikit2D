#include "InteractionSystems.h"
#include "InteractionComponents.h"

InteractionSystem::InteractionSystem() {

}

void InteractionSystem::update() {
	for (auto entity : _entityManager->getEntities()) {
		auto interactionObjects = entity->getComponent<InteractionComponent>();
		if (interactionObjects) {
			std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > operations = interactionObjects->getOperations();
			for (auto operation : operations) {
				if (std::get<1>(operation) == InteractionType::COMMON && std::get<0>(operation)->checkOperation()) {
					for (auto action : std::get<0>(operation)->getActions()) {
						action->doAction();
					}
				}
			}
		}
	}
}