#include "UISystem.h"
#include "ComponentFunctors.h"

std::tuple<std::tuple<int, int>, ClickCount> MouseSystem::processClickClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickClickMoveComponent> clickClickMoveComponent,
	std::shared_ptr<TransformComponent> transformComponent) {
	int currentClickX = std::get<0>(clickClickMoveComponent->_currentClick);
	int currentClickY = std::get<1>(clickClickMoveComponent->_currentClick);
	std::tuple<std::tuple<int, int>, ClickCount> clickedInside = { {0, 0}, ClickCount::NO };

	//first click
	if (clickClickMoveComponent->_clickFlag && currentClickX > objectComponent->_sceneX  && currentClickY > objectComponent->_sceneY &&
		currentClickX < objectComponent->_sceneX + objectComponent->_objectWidth && currentClickY < objectComponent->_sceneY + objectComponent->_objectHeight) {

		clickClickMoveComponent->_previousClick = clickClickMoveComponent->_currentClick;
		clickClickMoveComponent->_currentClick = { 0, 0 };
		clickClickMoveComponent->_clickFlag = false;
		clickedInside = { clickClickMoveComponent->_previousClick, ClickCount::FIRST };
		return clickedInside;
	}

	int previousClickX = std::get<0>(clickClickMoveComponent->_previousClick);
	int previousClickY = std::get<1>(clickClickMoveComponent->_previousClick);

	if (clickClickMoveComponent->_clickFlag && previousClickX && previousClickY) {
		clickClickMoveComponent->_previousClick = { 0, 0 };
		clickedInside = { clickClickMoveComponent->_currentClick , ClickCount::SECOND };
	}

	clickClickMoveComponent->_currentClick = { 0, 0 };
	clickClickMoveComponent->_clickFlag = false;

	return clickedInside;
}

void MouseSystem::processClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickMoveComponent> clickMoveComponent,
	std::shared_ptr<TransformComponent> transformComponent) {
	float speed = clickMoveComponent->_speed;
	int clickX = std::get<0>(clickMoveComponent->_leftClick);
	int clickY = std::get<1>(clickMoveComponent->_leftClick);
	if (!clickX || !clickY)
		return;
	if (clickMoveComponent->_move == false && clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight)
		return;

	clickMoveComponent->_move = true;
	int objectX = objectComponent->_sceneX + objectComponent->_objectWidth / 2;
	int objectY = objectComponent->_sceneY + objectComponent->_objectHeight / 2;
	float angle = atan2(clickY - objectY, clickX - objectX);
	float stepX = cos(angle) * speed;
	float stepY = sin(angle) * speed;
	if (abs(objectX - clickX) > speed || abs(objectY - clickY) > speed) {
		transformComponent->setTransform({ stepX, stepY });
	}
	else {
		clickMoveComponent->_move = false;
	}
}

std::tuple<std::tuple<int, int>, ClickCount> MouseSystem::processClickInside(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickInsideComponent> clickInsideComponent,
	std::shared_ptr<GroupEntitiesComponent> groupComponent) {
	int clickX = std::get<0>(clickInsideComponent->_leftClick);
	int clickY = std::get<1>(clickInsideComponent->_leftClick);
	std::tuple<std::tuple<int, int>, ClickCount> clickedInside = { {0, 0}, ClickCount::NO };

	if (!clickX || !clickY)
		return clickedInside;

	if (clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight &&
		clickInsideComponent->_leftClickFlag) {
		clickInsideComponent->_leftClickFlag = false;
		clickedInside = {clickInsideComponent->_leftClick, ClickCount::FIRST};
	}

	//We should handle click inside action only once per click, so reset coords of click after first handle
	clickInsideComponent->_leftClick = { 0, 0 };
	clickInsideComponent->_rightClick = { 0, 0 };

	return clickedInside;
}

//TODO: DISABLE ONLY MOVEMENT SYSTEM IF CLICKED
//THINK ABOUT SEVERAL CLICKTOMOVE ENTITIES
//DO MORE APROPRIATE LOGIC FOR HANDLING mainHeroDisableMoving (NOW ONLY 1 is disallowed but it's not correct so GG moves to second click)
void MouseSystem::update() {
	std::vector<std::shared_ptr<Entity> > playerControlledEntities;
	int playerControlledEntitiesDisableMoving = 0;

	//first of all we should find GG
	for (auto entity : getEntities()) {
		auto clickMoveComponent = entity->getComponent<ClickMoveComponent>();
		//Can use existence of ClickMoveComponent also
		if (clickMoveComponent)
			playerControlledEntities.push_back(entity);
	}

	//handle all components except ClickMoveComponent
	for (auto entity : getEntities()) {
		auto objectComponent = entity->getComponent<ObjectComponent>();
		auto clickInsideComponent = entity->getComponent<ClickInsideComponent>();
		auto groupComponent = entity->getComponent<GroupEntitiesComponent>();
		auto interactionComponent = entity->getComponent<InteractionAddToEntityComponent>();
		auto textureManagerComponent = entity->getComponent<TextureManagerComponent>();

		if (objectComponent && clickInsideComponent && groupComponent) {			
			int clickedInside = std::get<1>(processClickInside(objectComponent, clickInsideComponent, groupComponent));
			if (clickedInside) {
				std::cout << "Clicked inside: entityID " << entity->_index << " group " << groupComponent->_groupNumber 
					      << " " << groupComponent->_groupName << " programID " << objectComponent->_program << std::endl;
			}
			if (clickInsideComponent->_moveToByClick == false)
				playerControlledEntitiesDisableMoving += clickedInside;
			if (interactionComponent && clickedInside)
				interactionComponent->_interactReady = true;
			if (textureManagerComponent && clickedInside)
				textureManagerComponent->_interactReady = true;
		}

		auto transformComponent = entity->getComponent<TransformComponent>();
		auto clickClickMoveComponent = entity->getComponent<ClickClickMoveComponent>();
		auto interactionCreateEntityComponent = entity->getComponent<InteractionCreateEntityComponent>();

		if (objectComponent && clickClickMoveComponent && transformComponent) {
			auto clickedInsideTuple = processClickClickMove(objectComponent, clickClickMoveComponent, transformComponent);
			int clickedInside = std::get<1>(clickedInsideTuple);

			if (clickClickMoveComponent->_moveToByClickFirst == false && clickedInside == ClickCount::FIRST)
				playerControlledEntitiesDisableMoving += clickedInside;
			if (clickClickMoveComponent->_moveToByClickSecond == false && clickedInside == ClickCount::SECOND)
				playerControlledEntitiesDisableMoving += clickedInside;

			if (interactionComponent && clickedInside == ClickCount::SECOND)
				interactionComponent->_interactReady = true;
			if (interactionComponent && clickedInside == ClickCount::FIRST)
				interactionComponent->_interactReady = false;

			if (interactionCreateEntityComponent && clickedInside == ClickCount::SECOND) {
				interactionCreateEntityComponent->creationCoords = std::get<0>(clickedInsideTuple);
				interactionCreateEntityComponent->_interactReady = true;
			}
			if (interactionCreateEntityComponent && clickedInside == ClickCount::FIRST) {
				interactionCreateEntityComponent->creationCoords = { 0, 0 };
				interactionCreateEntityComponent->_interactReady = false;
			}
		}
	}

	for (auto entity : playerControlledEntities) {
		auto objectComponent = entity->getComponent<ObjectComponent>();
		auto clickMoveComponent = entity->getComponent<ClickMoveComponent>();
		auto transformComponent = entity->getComponent<TransformComponent>();

		if (objectComponent && clickMoveComponent && transformComponent && !playerControlledEntitiesDisableMoving)
			processClickMove(objectComponent, clickMoveComponent, transformComponent);
		else if (playerControlledEntitiesDisableMoving) {
			if (clickMoveComponent) {
				clickMoveComponent->_leftClick = { 0, 0 };
				clickMoveComponent->_rightClick = { 0, 0 };
			}
		}
	}
}

void InteractionAddToSystem::processManageTextures() {
	for (auto entity : getEntities()) {
		auto textureManagerComponent = entity->getComponent<TextureManagerComponent>();
		if (!textureManagerComponent)
			continue;
		if (textureManagerComponent->_interactReady) {
			int action = 0;
			std::cout << "Enter the 1 to get Textures list or 2 to add Texture, 0 to do nothing" << std::endl;
			std::cin >> action;
			switch (action) {
			case 0:
				break;
			case 1:
				textureManagerComponent->_textureManager->printTextures();
				break;
			case 2:
				std::string fullPath;
				std::cout << "Enter full path to image" << std::endl;
				std::cin >> fullPath;
				int atlasID, x, y;
				std::cout << "Enter atlas ID, position x, y in atlas" << std::endl;
				std::cin >> atlasID >> x >> y;
				int textureMode;
				std::cout << "Enter 1 if Texture, 2 if AnimatedTexture" << std::endl;
				std::cin >> textureMode;
				if (textureMode == 1) {
					textureManagerComponent->_textureManager->loadTexture(fullPath, atlasID, x, y);
				} else if (textureMode == 2) {
					int tileX, tileY;
					std::cout << "Enter x tiles number and y tiles number" << std::endl;
					std::cin >> tileX >> tileY;
					textureManagerComponent->_textureManager->loadTexture(fullPath, atlasID, x, y, tileX, tileY);
				}
				break;
			}
			textureManagerComponent->_interactReady = false;
		}
	}
}

void InteractionAddToSystem::processCreateEntity() {
	//

	for (auto entity : getEntities()) {
		auto interactionComponent = entity->getComponent<InteractionCreateEntityComponent>();
		if (!interactionComponent)
			continue;
		if (interactionComponent->_interactReady) {
			int action = 0;
			std::cout << "Enter the 1 to delete or 2 to create Entity, 0 to do nothing" << std::endl;
			std::cin >> action;
			switch (action) {
				case 0:
				break;
				case 1:
					int entityID;
					std::cout << "Enter ID of entity to delete" << std::endl;
					std::cin >> entityID;
					interactionComponent->_removeFunctor(entityID);
				break;
				case 2:
					interactionComponent->_createFunctor(interactionComponent->creationCoords, interactionComponent->creationSize);
				break;
			}
			interactionComponent->_interactReady = false;
		}
	}
}

void InteractionAddToSystem::processAddComponentToEntity() {
	shared_ptr<Entity> subjectEntity = nullptr;
	shared_ptr<Entity> objectEntity = nullptr;
	//find interaction components with subject and object types
	for (auto entity : getEntities()) {
		auto interactionComponent = entity->getComponent<InteractionAddToEntityComponent>();
		if (interactionComponent && interactionComponent->_interactReady) {
			if (interactionComponent->_interactionMember == InteractionMember::OBJECT)
				objectEntity = entity;
			else if (interactionComponent->_interactionMember == InteractionMember::SUBJECT)
				subjectEntity = entity;
		}
	}
	if (objectEntity && subjectEntity) {
		int action = 0;
		std::cout << "Enter the 1 to delete or 2 to add Component, 0 to do nothing" << std::endl;
		std::cin >> action;

		auto interactionComponentSubject = subjectEntity->getComponent<InteractionAddToEntityComponent>();

		switch (action) {
			case 0:
			break;
			case 1:
			{
				std::cout << "Choose component" << std::endl;
				for (std::map<std::string, std::shared_ptr<ComponentFunctor> >::iterator it = componentFunctors.begin(); it != componentFunctors.end(); ++it) {
					cout << "Name " << it->first << "\n";
				}
				std::string key;
				std::cin >> key;

				componentFunctors[key]->removeFunctor(objectEntity);
				break;
			}
			case 2:
			{
				std::cout << "Choose component" << std::endl;
				for (std::map<std::string, std::shared_ptr<ComponentFunctor> >::iterator it = componentFunctors.begin(); it != componentFunctors.end(); ++it) {
					cout << "Name " << it->first << "\n";
				}
				std::string key;
				std::cin >> key;

				std::shared_ptr<Component> addedComponent = componentFunctors[key]->createFunctor();
				objectEntity->addComponent(addedComponent);
				break;
			}
		}
	}

	//We should handle only if BOTH entities subject and object are ready for interaction
	if (objectEntity) {
		auto interactionComponentObject = objectEntity->getComponent<InteractionAddToEntityComponent>();
		if (interactionComponentObject)
			interactionComponentObject->_interactReady = false;
	}

	if (subjectEntity) {
		auto interactionComponentSubject = subjectEntity->getComponent<InteractionAddToEntityComponent>();
		if (interactionComponentSubject)
			interactionComponentSubject->_interactReady = false;
	}
}

void InteractionAddToSystem::update() {
	processAddComponentToEntity();
	processCreateEntity();
	processManageTextures();
}