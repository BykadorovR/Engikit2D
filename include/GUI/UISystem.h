#pragma once
#include "System.h"
#include "GraphicComponent.h"
#include "UIComponent.h"
#include "MoveComponent.h"


class UISystem : public System {
	void process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<UserInteractionComponent> UIComponent);
public:
	// Called every game update
	void update();

};

class MoveSystem : public System {
	void process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<UserInteractionComponent> UIComponent,
				 std::shared_ptr<TransformComponent> transformComponent);
public:
	void update();
};