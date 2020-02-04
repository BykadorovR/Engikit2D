#include "UserInputSystems.h"
#include "Operation.h"
#include "InteractionComponents.h"
#include "UserInputComponents.h"

MouseSystem::MouseSystem() {
	MouseEvent::instance().registerComponent(this);
}

bool MouseSystem::checkOperation() {
	for (auto entity : _entityManager->getEntities()) {
		auto interactionObjects = entity->getComponent<InteractionComponent>();
		if (interactionObjects) {
			std::vector<std::tuple<std::shared_ptr<Operation>, InteractionType> > operations = interactionObjects->getOperations();
			for (auto operation : operations) {
				if (std::get<1>(operation) == InteractionType::MOUSE && std::get<0>(operation)->checkOperation()) {
					for (auto action : std::get<0>(operation)->getActions()) {
						action->doAction();
					}
				}
			}
		}
	}
	//TODO: Add enumeration for return values
	return false;
}

void MouseSystem::mouseClickDownLeft(int x, int y) {
	//first update coords in mouse components
	for (auto entity : _entityManager->getEntities()) {
		auto mouseComponent = entity->getComponent<MouseComponent>();
		if (mouseComponent) {
			mouseComponent->setMember("leftClickX", x);
			mouseComponent->setMember("leftClickY", y);
		}
	}

	//check mouse related operations
	checkOperation();
}

void MouseSystem::mouseClickDownRight(int x, int y) {
}

MouseSystem::~MouseSystem() {
	MouseEvent::instance().unregisterComponent(this);
}
