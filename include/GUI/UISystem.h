#pragma once
#include "System.h"
#include "GraphicComponent.h"
#include "UIComponent.h"


class ClickInsideSystem : public System {
	void process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickInsideComponent> clickComponent, 
				 std::shared_ptr<GroupEntitiesComponent> groupComponent);
public:
	// Called every game update
	void update();

};

class PointMoveSystem : public System {
	void process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<PointMoveComponent> UIComponent,
				 std::shared_ptr<TransformComponent> transformComponent);
public:
	void update();
};

class ClickToMoveSystem : public System {
	void process(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickToMoveComponent> clickToMoveComponent,
				 std::shared_ptr<TransformComponent> transformComponent);
public:
	void update();
};