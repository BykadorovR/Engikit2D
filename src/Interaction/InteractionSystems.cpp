#include "GraphicComponents.h"
#include "InteractionSystems.h"
#include "InteractionComponents.h"

void InteractionSystem::update(InteractionType type) {
	for (auto entity : _entityManager->getEntities()) {
		if (std::get<1>(entity) == EntityState::ENTITY_UNREGISTERED)
			continue;

		auto interactionObjects = std::get<0>(entity)->getComponent<InteractionComponent>();
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
		//Part of code should be executed even if entity isn't registered
		auto objectComponent = std::get<0>(entity)->getComponent<ObjectComponent>();
		if (objectComponent) {
			//if not visible and registered
			bool value = *std::get<0>(objectComponent->getMemberFloat("visible"));
			if (value == false && std::get<1>(entity) == EntityState::ENTITY_REGISTERED) {
				_entityManager->unregisterEntity(std::get<0>(entity));
			}
			else if (value && std::get<1>(entity) == EntityState::ENTITY_UNREGISTERED) {
				_entityManager->registerEntity(std::get<0>(entity));
			}
		}
		//
		if (std::get<1>(entity) == EntityState::ENTITY_UNREGISTERED)
			continue;
	}
}