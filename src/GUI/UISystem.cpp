#include "UISystem.h"
#include "ComponentFunctors.h"
#include "GUISave.h"

std::tuple<std::tuple<int, int>, ClickCount> MouseSystem::processClickClickMove(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<ClickClickMoveComponent> clickClickMoveComponent) {
	float currentClickX = std::get<0>(clickClickMoveComponent->_currentClick);
	float currentClickY = std::get<1>(clickClickMoveComponent->_currentClick);
	std::tuple<std::tuple<int, int>, ClickCount> clickedInside = { {0, 0}, ClickCount::NO };

	//first click
	if (clickClickMoveComponent->_clickFlag && currentClickX > objectComponent->getSceneX()  && currentClickY > objectComponent->getSceneY() &&
		currentClickX < objectComponent->getSceneX() + objectComponent->getWidth() && currentClickY < objectComponent->getSceneY() + objectComponent->getHeight()) {

		clickClickMoveComponent->_previousClick = clickClickMoveComponent->_currentClick;
		clickClickMoveComponent->_currentClick = { 0, 0 };
		clickClickMoveComponent->_clickFlag = false;
		clickedInside = { clickClickMoveComponent->_previousClick, ClickCount::FIRST };
		return clickedInside;
	}

	float previousClickX = std::get<0>(clickClickMoveComponent->_previousClick);
	float previousClickY = std::get<1>(clickClickMoveComponent->_previousClick);

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
	moveComponent->setTransform({ moveComponent->_direction.first * speed, moveComponent->_direction.second * speed });

	float clickX = std::get<0>(moveComponent->_leftClick);
	float clickY = std::get<1>(moveComponent->_leftClick);
	if (!clickX || !clickY)
		return;
	if (moveComponent->_move == false && clickX > objectComponent->getSceneX()  && clickY > objectComponent->getSceneY() &&
		clickX < objectComponent->getSceneX() + objectComponent->getWidth() && clickY < objectComponent->getSceneY() + objectComponent->getHeight())
		return;

	moveComponent->_move = true;
	float objectX = objectComponent->getSceneX() + objectComponent->getWidth() / 2;
	float objectY = objectComponent->getSceneY() + objectComponent->getHeight() / 2;
	float angle = atan2(clickY - objectY, clickX - objectX);
	float stepX = cos(angle) * speed;
	float stepY = sin(angle) * speed;
	if (abs(objectX - clickX) > speed || abs(objectY - clickY) > speed) {
		moveComponent->setTransform({ stepX, stepY });
	} else {
		moveComponent->_move = false;
		moveComponent->_coords = { 0, 0 };
		moveComponent->_leftClick = { 0, 0 };
		moveComponent->_rightClick = { 0, 0 };
	}
}

void CameraSystem::moveEntity(std::shared_ptr<ObjectComponent> objectComponent, std::shared_ptr<CameraComponent> cameraComponent) {
	float speed = cameraComponent->_cameraSpeed;
	float clickX = std::get<0>(cameraComponent->_leftClick);
	float clickY = std::get<1>(cameraComponent->_leftClick);
	if (!clickX || !clickY)
		return;

	clickX -= cameraComponent->_cameraX;
	clickY -= cameraComponent->_cameraY;

	if (cameraComponent->_move == false && clickX > objectComponent->getSceneX()  && clickY > objectComponent->getSceneY() &&
		clickX < objectComponent->getSceneX() + objectComponent->getWidth() && clickY < objectComponent->getSceneY() + objectComponent->getHeight())
		return;

	cameraComponent->_move = true;
	float objectX = objectComponent->getSceneX() + objectComponent->getWidth() / 2;
	float objectY = objectComponent->getSceneY() + objectComponent->getHeight() / 2;
	float angle = atan2(clickY - objectY, clickX - objectX);
	float stepX = cos(angle) * speed;
	float stepY = sin(angle) * speed;
	if (abs(objectX - clickX) > speed || abs(objectY - clickY) > speed) {
		cameraComponent->setTransform({ stepX, stepY });
	}
	else {
		cameraComponent->_move = false;
		cameraComponent->_coords = { 0, 0 };
		cameraComponent->_cameraX = 0;
		cameraComponent->_cameraY = 0;
		cameraComponent->_leftClick = { 0, 0 };
		cameraComponent->_rightClick = { 0, 0 };
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

	if (clickX > objectComponent->getSceneX()  && clickY > objectComponent->getSceneY() &&
		clickX < objectComponent->getSceneX() + objectComponent->getWidth() && clickY < objectComponent->getSceneY() + objectComponent->getHeight() &&
		clickInsideComponent->_leftClickFlag) {
		clickInsideComponent->_leftClickFlag = false;
		clickedInside = {clickInsideComponent->_leftClick, ClickCount::FIRST};
	}

	//We should handle click inside action only once per click, so reset coords of click after first handle
	clickInsideComponent->_leftClick = { 0, 0 };
	clickInsideComponent->_rightClick = { 0, 0 };
	std::get<1>(clickedInside) = ClickCount::FIRST;

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

	bool skipNextClickInside = false;

	//handle all components except ClickMoveComponent
	for (auto entity : entityManager->getEntities()) {
		auto objectComponent = entity->getComponent<ObjectComponent>();
		auto clickInsideComponent = entity->getComponent<ClickInsideComponent>();
		auto groupComponent = entity->getComponent<GroupEntitiesComponent>();
		auto interactionComponent = entity->getComponent<InteractionAddToEntityComponent>();
		auto textureManagerComponent = entity->getComponent<TextureManagerComponent>();
		auto saveLoadComponent = entity->getComponent<SaveLoadComponent>();
		auto textComponent = entity->getComponent<TextComponent>();

		if (groupComponent->_groupName == "Engine" && gameMode == true)
			continue;

		if (objectComponent && clickInsideComponent && groupComponent) {
			std::tuple<std::tuple<int, int>, int> click = (processClickInside(objectComponent, clickInsideComponent, groupComponent));
			bool clickedInside = (std::get<0>(std::get<0>(click)) != 0 || std::get<1>(std::get<0>(click)) != 0) ? true : false;
			//workaround to avoid clicking to sprite and triggering event in case of clickclickmove second click
			if (textComponent && clickedInside && textComponent->_type == TextType::EDIT) {
				textComponent->setFocus(true);
				//let's clear the text to avoid backspace
				textComponent->_text = "";
			}
			if (clickedInside && !skipNextClickInside) {
				//avoid click through several objects
				OUT_STREAM("Clicked inside: entityID " << entity->_index << " group " << groupComponent->_groupNumber
					<< " " << groupComponent->_groupName << " programID " << objectComponent->_program << std::endl);

				if (!textComponent || textComponent->_type == TextType::LABEL)
					for (auto &instance : TextHelper::instance()->_buffer) {
						TextHelper::instance()->detachText(instance);
					}
				if (clickInsideComponent->_event.first) {
					//to prohibit edit text trigger events which were inhereted from previously attached sprites
					if (!textComponent || textComponent->_type == TextType::LABEL) {
						clickInsideComponent->_event.first->configureFunctor(clickInsideComponent->_event.second);
					}
				}
			}
			else if (std::get<1>(click) != ClickCount::NO) {
				if (textComponent)
					textComponent->setFocus(false);
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
						cameraComponent->_cameraX = 0;
						cameraComponent->_cameraY = 0;
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
				if (!textComponent || textComponent->_type == TextType::LABEL)
					for (auto &instance : TextHelper::instance()->_buffer) {
						TextHelper::instance()->detachText(instance);
					}
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
						cameraComponent->_cameraX = 0;
						cameraComponent->_cameraY = 0;
					}
				}
			}
			if (clickClickMoveComponent->_moveToByClickSecond == false && clickedInside == ClickCount::SECOND) {
				skipNextClickInside = true;
				if (!textComponent || textComponent->_type == TextType::LABEL)
					for (auto &instance : TextHelper::instance()->_buffer) {
						TextHelper::instance()->detachText(instance);
					}
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
						cameraComponent->_cameraX = 0;
						cameraComponent->_cameraY = 0;
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
			//TODO: path to texture?
			int index = 0;
			int x = TextHelper::instance()->getX();
			int y = TextHelper::instance()->getY();
			int height = TextHelper::instance()->getHeight();
			int width = TextHelper::instance()->getWidth();
			float size = TextHelper::instance()->getSize();
			auto entity = TextHelper::instance()->createText("Texture list", x, y + height * index++, width, height, size, false);
			std::shared_ptr<ClickInsideComponent> clickInsideComponent = entity->getComponent<ClickInsideComponent>();
			std::shared_ptr<TextureListEvent> functor = std::make_shared<TextureListEvent>();
			clickInsideComponent->_event = std::make_pair(functor, nullptr);
			TextHelper::instance()->attachText(entity);

			entity = TextHelper::instance()->createText("Add texture", x, y + height * index++, width, height, size, false);
			clickInsideComponent = entity->getComponent<ClickInsideComponent>();
			std::shared_ptr<TextureLoadEvent> functorLoad = std::make_shared<TextureLoadEvent>();
			clickInsideComponent->_event = std::make_pair(functorLoad, nullptr);
			TextHelper::instance()->attachText(entity);

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
			interactionComponent->_createFunctor(interactionComponent->creationCoords);
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
		int x = TextHelper::instance()->getX();
		int y = TextHelper::instance()->getY();
		int height = TextHelper::instance()->getHeight();
		int width = TextHelper::instance()->getWidth();
		float size = TextHelper::instance()->getSize();
		int index = 0;
		for (auto &functors : componentFunctors) {
			std::string name = functors.first;
			//To avoid adding components which are always should be in entity
			if (!addFunctors[name])
				continue;
			auto entity = TextHelper::instance()->createText(name, x, y + height * index++, width, height, size, false);
			std::shared_ptr<ClickInsideComponent> clickInsideComponent = entity->getComponent<ClickInsideComponent>();
			std::shared_ptr<AddComponentEvent> functor = std::make_shared<AddComponentEvent>();
			functor->_name = name;
			clickInsideComponent->_event = std::make_pair(functor, objectEntity);
			TextHelper::instance()->attachText(entity);
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
				if (targetEntity == nullptr) {
					targetEntity = entityManager->create();
					//targetEntity->_index = id;
				}

				std::vector<std::shared_ptr<ComponentFunctor> > repeat;
				//TODO: REDO IN NORMAL WAY
				for (auto functor : componentFunctors) {
					int sts = functor.second->deserializeFunctor(targetEntity, itID.value());
					if (sts == 1) {
						repeat.push_back(functor.second);
					}
				}
				//
				for (auto functor : repeat) {
					functor->deserializeFunctor(targetEntity, itID.value());
				}
			}
		}
	}
}

void SaveLoadSystem::saveResolution(std::shared_ptr<GUISave> fileSave) {
	fileSave->_jsonFile["Screen resolution"] = { resolution.first, resolution.second };
}

void SaveLoadSystem::loadResolution(std::shared_ptr<GUISave> fileLoad) {
	for (json::iterator it = fileLoad->_jsonFile.begin(); it != fileLoad->_jsonFile.end(); ++it) {
		if (it.key() == "Screen resolution") {
			int resolutionX = it.value()[0];
			int resolutionY = it.value()[1];
			resolutionRatioX = (float) resolution.first / (float) resolutionX;
			resolutionRatioY = (float) resolution.second / (float)resolutionY;
		}
	}
}

void SaveLoadSystem::saveTextures(std::shared_ptr<GUISave> fileSave) {
	auto textureManager = TextureManager::instance();
	//texture atlases:
	//width height atlasID
	//textures: path width height row column atlasID posXAtlas posYAtlas
	//
	for (int i = 0; i < textureManager->getAtlasList().size(); i++) {
		auto textureAtlas = textureManager->getAtlasList()[i];
		fileSave->_jsonFile["textureAtlas"][std::to_string(i)] = { textureAtlas->getAtlasID(), textureAtlas->getWidth(), textureAtlas->getHeight() };
	}
	for (int i = 0; i < textureManager->getTextureList().size(); i++) {
		auto texture = textureManager->getTextureList()[i];
		fileSave->_jsonFile["texture"][std::to_string(i)] = { texture->getTextureID(), texture->getPath(), texture->getAtlas()->getAtlasID(), texture->getPosXAtlas(), texture->getPosYAtlas(),
										   texture->getRow(), texture->getColumn() };
	}
	fileSave->saveToFile();
}

void SaveLoadSystem::loadTextures(std::shared_ptr<GUISave> fileLoad) {
	auto textureManager = TextureManager::instance();
	for (json::iterator it = fileLoad->_jsonFile.begin(); it != fileLoad->_jsonFile.end(); ++it) {
		if (it.key() == "textureAtlas") {
			for (auto itID = it.value().begin(); itID != it.value().end(); ++itID) {
				textureManager->loadAtlas(itID.value()[0], itID.value()[1], itID.value()[2]);
			}
		}
	}
	for (json::iterator it = fileLoad->_jsonFile.begin(); it != fileLoad->_jsonFile.end(); ++it) {
		if (it.key() == "texture") {
			for (auto itID = it.value().begin(); itID != it.value().end(); ++itID) {
				auto texture = textureManager->loadTexture(itID.value()[1], itID.value()[2], itID.value()[3], itID.value()[4], itID.value()[6], itID.value()[5]);
				texture->setTextureID(itID.value()[0]);
			}
		}
	}
}

void SaveLoadSystem::update(shared_ptr<EntityManager> entityManager) {
	std::string fileName;
	//find save sprite and check interaction component is ready
	for (auto targetEntity : entityManager->getEntities()) {
		auto saveLoadComponent = targetEntity->getComponent<SaveLoadComponent>();
		if (saveLoadComponent) {
			if (saveLoadComponent->_mode == 0 && saveLoadComponent->_path != "") {
				saveLoadComponent->_mode = 2;
				std::shared_ptr<GUISave> saveFile = std::make_shared<GUISave>(saveLoadComponent->_path);
				//First of all need to save texture atlases and textures
				saveResolution(saveFile);
				saveTextures(saveFile);
				saveEntities(entityManager, saveFile);
				saveLoadComponent->_path = "";
			}
			if (saveLoadComponent->_mode == 1 && saveLoadComponent->_path != "") {
				saveLoadComponent->_mode = 2;
				std::shared_ptr<GUISave> loadFile = make_shared<GUISave>(saveLoadComponent->_path);
				loadFile->loadFile();
				loadResolution(loadFile);
				loadTextures(loadFile);
				loadEntities(entityManager, loadFile);
				saveLoadComponent->_path = "";
			}
		}
		if (saveLoadComponent && saveLoadComponent->_interactReady) {
			int index = 0;
			int x = TextHelper::instance()->getX();
			int y = TextHelper::instance()->getY();
			int height = TextHelper::instance()->getHeight();
			int width = TextHelper::instance()->getWidth();
			float size = TextHelper::instance()->getSize();
			auto entity = TextHelper::instance()->createText("Save scene", x, y + height * index++, width, height, size, false);
			std::shared_ptr<ClickInsideComponent> clickInsideComponent = entity->getComponent<ClickInsideComponent>();
			std::shared_ptr<SaveEvent> functor = std::make_shared<SaveEvent>();
			clickInsideComponent->_event = std::make_pair(functor, targetEntity);
			TextHelper::instance()->attachText(entity);

			entity = TextHelper::instance()->createText("Load scene", x, y + height * index++, width, height, size, false);
			clickInsideComponent = entity->getComponent<ClickInsideComponent>();
			std::shared_ptr<LoadEvent> functorLoad = std::make_shared<LoadEvent>();
			clickInsideComponent->_event = std::make_pair(functorLoad, targetEntity);
			TextHelper::instance()->attachText(entity);
			saveLoadComponent->_interactReady = false;
		}
	}

}
