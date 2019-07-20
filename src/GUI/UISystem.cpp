#include "UISystem.h"
#include "ComponentFunctors.h"
#include "GUISave.h"

std::tuple<std::tuple<int, int>, ClickCount> MouseSystem::processClickClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickClickMoveComponent> clickClickMoveComponent) {
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

void MoveSystem::moveEntity(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<MoveComponent> moveComponent) {
	float speed = moveComponent->_speed;
	int clickX = std::get<0>(moveComponent->_leftClick);
	int clickY = std::get<1>(moveComponent->_leftClick);
	if (!clickX || !clickY)
		return;
	if (moveComponent->_move == false && clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight)
		return;

	moveComponent->_move = true;
	int objectX = objectComponent->_sceneX + objectComponent->_objectWidth / 2;
	int objectY = objectComponent->_sceneY + objectComponent->_objectHeight / 2;
	float angle = atan2(clickY - objectY, clickX - objectX);
	float stepX = cos(angle) * speed;
	float stepY = sin(angle) * speed;
	if (abs(objectX - clickX) > speed || abs(objectY - clickY) > speed) {
		moveComponent->setTransform({ stepX, stepY });
	} else {
		moveComponent->_move = false;
		moveComponent->_coords = { 0, 0 };
	}
}

void CameraSystem::moveEntity(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<CameraComponent> cameraComponent) {
	float speed = cameraComponent->_cameraSpeed;
	int clickX = std::get<0>(cameraComponent->_leftClick);
	int clickY = std::get<1>(cameraComponent->_leftClick);
	if (!clickX || !clickY)
		return;
	if (cameraComponent->_move == false && clickX > objectComponent->_sceneX  && clickY > objectComponent->_sceneY &&
		clickX < objectComponent->_sceneX + objectComponent->_objectWidth && clickY < objectComponent->_sceneY + objectComponent->_objectHeight)
		return;

	cameraComponent->_move = true;
	int objectX = objectComponent->_sceneX + objectComponent->_objectWidth / 2;
	int objectY = objectComponent->_sceneY + objectComponent->_objectHeight / 2;
	float angle = atan2(clickY - objectY, clickX - objectX);
	float stepX = cos(angle) * speed;
	float stepY = sin(angle) * speed;
	if (abs(objectX - clickX) > speed || abs(objectY - clickY) > speed) {
		cameraComponent->setTransform({ stepX, stepY });
	}
	else {
		cameraComponent->_move = false;
		cameraComponent->_coords = { 0, 0 };
	}
}


void MoveSystem::update(std::shared_ptr<EntityManager> entityManager) {
	for (auto entity : entityManager->getEntities()) {
		auto objectComponent = entity->getComponent<ObjectComponent>();
		auto moveComponent = entity->getComponent<MoveComponent>();

		if (objectComponent && moveComponent) {
			moveEntity(objectComponent, moveComponent);
		}
	}
}

void CameraSystem::update(std::shared_ptr<EntityManager> entityManager) {
	for (auto entity : entityManager->getEntities()) {
		auto objectComponent = entity->getComponent<ObjectComponent>();
		auto cameraComponent = entity->getComponent<CameraComponent>();

		if (objectComponent && cameraComponent) {
			moveEntity(objectComponent, cameraComponent);
		}
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
void MouseSystem::update(shared_ptr<EntityManager> entityManager) {
	std::vector<std::shared_ptr<Entity> > playerControlledEntities;

	//first of all we should find GG
	for (auto entity : entityManager->getEntities()) {
		auto moveComponent = entity->getComponent<MoveComponent>();
		auto cameraComponent = entity->getComponent<CameraComponent>();
		//Can use existence of ClickMoveComponent also
		if ((moveComponent && moveComponent->_type == PlayerControlled) || cameraComponent)
			playerControlledEntities.push_back(entity);
	}

	//handle all components except ClickMoveComponent
	for (auto entity : entityManager->getEntities()) {
		auto objectComponent = entity->getComponent<ObjectComponent>();
		auto clickInsideComponent = entity->getComponent<ClickInsideComponent>();
		auto groupComponent = entity->getComponent<GroupEntitiesComponent>();
		auto interactionComponent = entity->getComponent<InteractionAddToEntityComponent>();
		auto textureManagerComponent = entity->getComponent<TextureManagerComponent>();
		auto saveLoadComponent = entity->getComponent<SaveLoadComponent>();

		if (objectComponent && clickInsideComponent && groupComponent) {			
			int clickedInside = std::get<1>(processClickInside(objectComponent, clickInsideComponent, groupComponent));
			if (clickedInside) {
				std::cout << "Clicked inside: entityID " << entity->_index << " group " << groupComponent->_groupNumber 
					      << " " << groupComponent->_groupName << " programID " << objectComponent->_program << std::endl;
			}
			if (clickedInside && clickInsideComponent->_moveToByClick == false) {
				for (auto playerEntity : playerControlledEntities) {
					auto moveComponent = playerEntity->getComponent<MoveComponent>();
					if (moveComponent) {
						moveComponent->_move = false;
						moveComponent->_coords = { 0, 0 };
						moveComponent->_leftClick = { 0, 0 };
						moveComponent->_rightClick = { 0, 0 };
					}
					auto cameraComponent = playerEntity->getComponent<CameraComponent>();
					if (cameraComponent) {
						cameraComponent->_move = false;
						cameraComponent->_coords = { 0, 0 };
						cameraComponent->_leftClick = { 0, 0 };
						cameraComponent->_rightClick = { 0, 0 };
					}
				}
			}
			if (interactionComponent && clickedInside)
				interactionComponent->_interactReady = true;
			if (textureManagerComponent && clickedInside)
				textureManagerComponent->_interactReady = true;
			if (saveLoadComponent && clickedInside)
				saveLoadComponent->_interactReady = true;
		}

		auto clickClickMoveComponent = entity->getComponent<ClickClickMoveComponent>();
		auto interactionCreateEntityComponent = entity->getComponent<InteractionCreateEntityComponent>();

		if (objectComponent && clickClickMoveComponent) {
			auto clickedInsideTuple = processClickClickMove(objectComponent, clickClickMoveComponent);
			int clickedInside = std::get<1>(clickedInsideTuple);

			if (clickClickMoveComponent->_moveToByClickFirst == false && clickedInside == ClickCount::FIRST) {
				for (auto playerEntity : playerControlledEntities) {
					auto moveComponent = playerEntity->getComponent<MoveComponent>();
					if (moveComponent) {
						moveComponent->_move = false;
						moveComponent->_coords = { 0, 0 };
						moveComponent->_leftClick = { 0, 0 };
						moveComponent->_rightClick = { 0, 0 };
					}
					auto cameraComponent = playerEntity->getComponent<CameraComponent>();
					if (cameraComponent) {
						cameraComponent->_move = false;
						cameraComponent->_coords = { 0, 0 };
						cameraComponent->_leftClick = { 0, 0 };
						cameraComponent->_rightClick = { 0, 0 };
					}
				}
			}
			if (clickClickMoveComponent->_moveToByClickSecond == false && clickedInside == ClickCount::SECOND) {
				for (auto playerEntity : playerControlledEntities) {
					auto moveComponent = playerEntity->getComponent<MoveComponent>();
					if (moveComponent) {
						moveComponent->_move = false;
						moveComponent->_coords = { 0, 0 };
						moveComponent->_leftClick = { 0, 0 };
						moveComponent->_rightClick = { 0, 0 };
					}
					auto cameraComponent = playerEntity->getComponent<CameraComponent>();
					if (cameraComponent) {
						cameraComponent->_move = false;
						cameraComponent->_coords = { 0, 0 };
						cameraComponent->_leftClick = { 0, 0 };
						cameraComponent->_rightClick = { 0, 0 };
					}
				}
			}

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
}

void InteractionAddToSystem::processManageTextures(shared_ptr<EntityManager> entityManager) {
	for (auto entity : entityManager->getEntities()) {
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
				TextureManager::instance()->printTextures();
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
					TextureManager::instance()->loadTexture(fullPath, atlasID, x, y);
				} else if (textureMode == 2) {
					int tileX, tileY;
					std::cout << "Enter x tiles number and y tiles number" << std::endl;
					std::cin >> tileX >> tileY;
					TextureManager::instance()->loadTexture(fullPath, atlasID, x, y, tileX, tileY);
				}
				break;
			}
			textureManagerComponent->_interactReady = false;
		}
	}
}

void InteractionAddToSystem::processCreateEntity(shared_ptr<EntityManager> entityManager) {
	//

	for (auto entity : entityManager->getEntities()) {
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
					interactionComponent->_createFunctor(interactionComponent->creationCoords);
				break;
			}
			interactionComponent->_interactReady = false;
		}
	}
}

void InteractionAddToSystem::processAddComponentToEntity(shared_ptr<EntityManager> entityManager) {
	shared_ptr<Entity> subjectEntity = nullptr;
	shared_ptr<Entity> objectEntity = nullptr;
	//find interaction components with subject and object types
	for (auto entity : entityManager->getEntities()) {
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

void InteractionAddToSystem::update(shared_ptr<EntityManager> entityManager) {
	processAddComponentToEntity(entityManager);
	processCreateEntity(entityManager);
	processManageTextures(entityManager);
}

void SaveLoadSystem::saveEntities(shared_ptr<EntityManager> entityManager, std::shared_ptr<GUISave> fileSave) {
	for (auto entity : entityManager->getEntities()) {
		auto groupEntitiesComponent = entity->getComponent<GroupEntitiesComponent>();
		if (groupEntitiesComponent && groupEntitiesComponent->_groupName != std::string("Engine"))
			for (auto functor : componentFunctors) {
				functor.second->serializeFunctor(entity, fileSave);
			}
	}
	fileSave->saveToFile();
}

void SaveLoadSystem::loadEntities(shared_ptr<EntityManager> entityManager, std::shared_ptr<GUISave> fileLoad) {
	for (json::iterator it = fileLoad->_jsonFile.begin(); it != fileLoad->_jsonFile.end(); ++it) {
		if (std::string("Entity") == it.key()) {
			for (json::iterator itID = it.value().begin(); itID != it.value().end(); ++itID) {
				std::shared_ptr<Entity> targetEntity = nullptr;
				int id = atoi(itID.key().c_str());
				for (auto entity : entityManager->getEntities()) {
					//it's entity ID
					if (id == entity->_index)
						targetEntity = entity;
				}
				if (targetEntity == nullptr) {
					targetEntity = entityManager->create();
					targetEntity->_index = id;
				}
				for (auto functor : componentFunctors) {
					functor.second->deserializeFunctor(targetEntity, itID.value());
				}
			}
		}
	}
}

void SaveLoadSystem::saveTextures(std::shared_ptr<GUISave> fileSave) {
	auto textureManager = TextureManager::instance();
	//texture atlases:
	//width height atlasID
	//textures: path width height row column atlasID posXAtlas posYAtlas
	//
	for (auto &textureAtlas : textureManager->getAtlasList()) {
		fileSave->_jsonFile["textureAtlas"] = { textureAtlas->getAtlasID(), textureAtlas->getWidth(), textureAtlas->getHeight() };
	}
	for (auto &texture : textureManager->getTextureList()) {
		fileSave->_jsonFile["texture"] = { texture->getTextureID(), texture->getPath(), texture->getAtlas()->getAtlasID(), texture->getPosXAtlas(), texture->getPosYAtlas(),
										   texture->getRow(), texture->getColumn() };
	}
	fileSave->saveToFile();
}

void SaveLoadSystem::loadTextures(std::shared_ptr<GUISave> fileLoad) {
	auto textureManager = TextureManager::instance();
	for (json::iterator it = fileLoad->_jsonFile.begin(); it != fileLoad->_jsonFile.end(); ++it) {
		if (it.key() == "textureAtlas") {
			textureManager->loadAtlas(it.value()[0], it.value()[1], it.value()[2]);
		}
	}
	for (json::iterator it = fileLoad->_jsonFile.begin(); it != fileLoad->_jsonFile.end(); ++it) {
		if (it.key() == "texture") {
			auto texture = textureManager->loadTexture(it.value()[1], it.value()[2], it.value()[3], it.value()[4], it.value()[6], it.value()[5]);
			texture->setTextureID(it.value()[0]);
		}
	}
}

void SaveLoadSystem::update(shared_ptr<EntityManager> entityManager) {
	std::string fileName;
	int mode = 2;
	//find save sprite and check interaction component is ready
	for (auto entity : entityManager->getEntities()) {
		auto saveLoadComponent = entity->getComponent<SaveLoadComponent>();
		if (saveLoadComponent && saveLoadComponent->_interactReady) {
			std::cout << "Enter 0 to save scene to file, 1 to load scene from file, 2 to do nothing" << std::endl;
			std::cin >> mode;
			if (mode == 2) {
				saveLoadComponent->_interactReady = false;
				return;
			}
			std::cout << "Enter filename" << std::endl;
			std::cin >> fileName;
			saveLoadComponent->_interactReady = false;
		}
	}

	if (mode == 0) {
		std::shared_ptr<GUISave> saveFile = std::make_shared<GUISave>(fileName);
		//First of all need to save texture atlases and textures
		saveTextures(saveFile);
		saveEntities(entityManager, saveFile);
	}
	if (mode == 1) {
		std::shared_ptr<GUISave> loadFile = make_shared<GUISave>(fileName);
		loadFile->loadFile();
		loadTextures(loadFile);
		loadEntities(entityManager, loadFile);
	}
}
