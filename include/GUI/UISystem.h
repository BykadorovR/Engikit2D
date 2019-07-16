#pragma once
#include "System.h"
#include "GraphicComponent.h"
#include "UIComponent.h"

enum ClickCount {
	NO = 0,
	FIRST = 1,
	SECOND = 2
};

class MoveSystem : public System {
	void MoveSystem::moveEntity(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<MoveComponent> moveComponent);

public:
	void update(shared_ptr<EntityManager> entityManager);
};

class MouseSystem : public System {
	std::tuple<std::tuple<int, int>, ClickCount> processClickClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickClickMoveComponent> clickClickMoveComponent);

	std::tuple<std::tuple<int, int>, ClickCount> processClickInside(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickInsideComponent> clickInsideComponent,
		std::shared_ptr<GroupEntitiesComponent> groupComponent);
public:
	void update(shared_ptr<EntityManager> entityManager);
};

class InteractionAddToSystem : public System {
	void processAddComponentToEntity(shared_ptr<EntityManager> entityManager);
	void processCreateEntity(shared_ptr<EntityManager> entityManager);
	void processManageTextures(shared_ptr<EntityManager> entityManager);
public:
	void update(shared_ptr<EntityManager> entityManager);
};

class SaveLoadSystem : public System {
	void saveEntities(shared_ptr<EntityManager> entityManager, std::shared_ptr<GUISave> fileSave);
	void loadEntities(shared_ptr<EntityManager> entityManager, std::shared_ptr<GUISave> fileLoad);
	void saveTextures(std::shared_ptr<GUISave> fileSave);
	void loadTextures(std::shared_ptr<GUISave> fileLoad);
public:
	void update(shared_ptr<EntityManager> entityManager);
};

class CameraSystem : public System {
public:
	void update(shared_ptr<EntityManager> entityManager);
};