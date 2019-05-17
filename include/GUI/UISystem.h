#pragma once
#include "System.h"
#include "GraphicComponent.h"
#include "UIComponent.h"

enum ClickCount {
	NO = 0,
	FIRST = 1,
	SECOND = 2
};

class MouseSystem : public System {
	std::tuple<std::tuple<int, int>, ClickCount> processClickClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickClickMoveComponent> clickClickMoveComponent,
		std::shared_ptr<TransformComponent> transformComponent);

	void processClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickMoveComponent> clickMoveComponent,
		std::shared_ptr<TransformComponent> transformComponent);

	std::tuple<std::tuple<int, int>, ClickCount> processClickInside(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickInsideComponent> clickInsideComponent,
		std::shared_ptr<GroupEntitiesComponent> groupComponent);
public:
	void update();
};

class InteractionAddToSystem : public System {
	void processAddComponentToEntity();
	void processCreateEntity();
	void processManageTextures();
public:
	void update();
};

