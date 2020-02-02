#include "InteractionSystems.h"
#include "InteractionComponents.h"

InteractionSystem::InteractionSystem() {

}

void InteractionSystem::update(std::shared_ptr<EntityManager> entityManager) {
	for (auto entity : entityManager->getEntities()) {
		auto interactionObjects = entity->getComponent<InteractionComponent>();
		if (interactionObjects) {
			std::vector<std::shared_ptr<Operation> > operations = interactionObjects->getOperations();
			for (auto operation : operations) {
				if (operation->checkOperation()) {
					for (auto action : operation->getActions()) {
						action->doAction();
					}
				}
			}
		}
	}
}