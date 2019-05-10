#pragma once
#include "System.h"
#include "GraphicComponent.h"
#include "UIComponent.h"

class MouseSystem : public System {
	int processClickClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickClickMoveComponent> clickClickMoveComponent,
		std::shared_ptr<TransformComponent> transformComponent);

	void processClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickMoveComponent> clickMoveComponent,
		std::shared_ptr<TransformComponent> transformComponent);

	int processClickInside(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickInsideComponent> clickInsideComponent,
		std::shared_ptr<GroupEntitiesComponent> groupComponent);
public:
	void update();
};

class InteractionAddToEntitySystem : public System {
	void process();
public:
	void update();
};