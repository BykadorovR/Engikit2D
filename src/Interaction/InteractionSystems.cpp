#include "GraphicComponents.h"
#include "InteractionSystems.h"
#include "InteractionComponents.h"

void InteractionSystem::update(InteractionType type) {
	for (auto entity : _entityManager->getEntities()) {
		auto interactionObjects = entity->getComponent<InteractionComponent>();
		if (interactionObjects) {
			std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > operations = interactionObjects->getOperations();
			for (auto operation : operations) {
				if (std::get<1>(operation) == type && std::get<0>(operation)->checkOperation()) {
					for (auto action : std::get<0>(operation)->getActions()) {
						action->doAction();
					}
				}
			}
		}
	}
}

void StateSystem::update(InteractionType type) {
	for (auto entity : _entityManager->getEntities()) {
		auto objectComponent = entity->getComponent<ObjectComponent>();
		if (objectComponent) {
			//if not visible and registered
			bool value = *std::get<0>(objectComponent->getMemberFloat("visible"));
			if (value == false && entity->getIndex() != -1) {
				_entityManager->unregisterEntity(entity);
			}
			else if (value && entity->getIndex() == -1) {
				_entityManager->registerEntity(entity);
			}
		}
	}
}