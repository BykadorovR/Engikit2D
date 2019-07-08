#pragma once
#include "Component.h"
#include "UIComponent.h"
#include "GraphicComponent.h"
#include "Texture.h"

class ComponentFunctor {
public:
	virtual std::shared_ptr<Component> createFunctor() = 0;
	virtual void removeFunctor(std::shared_ptr<Entity> targetEntity) = 0;
	virtual void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) = 0;
	virtual void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) = 0;
};

extern std::map<std::string, std::shared_ptr<ComponentFunctor> > componentFunctors;

class ClickMoveComponentFunctor : public ComponentFunctor {
	std::shared_ptr<Component> createFunctor() {
		std::shared_ptr<ClickMoveComponent> clickComponent(new ClickMoveComponent());
		int speed;
		std::cout << "Enter the speed" << std::endl;
		std::cin >> speed;
		clickComponent->initialize(speed);
		return clickComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<ClickMoveComponent>();
	}

	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<ClickMoveComponent> clickMoveComponent = targetEntity->getComponent<ClickMoveComponent>();
		if (!clickMoveComponent)
			return;

		save->_jsonFile["Entity"]["ID"] = entityID;
		save->_jsonFile["Entity"]["ClickMoveComponent"]["leftClick"] = { std::get<0>(clickMoveComponent->_leftClick), std::get<1>(clickMoveComponent->_leftClick) };
		save->_jsonFile["Entity"]["ClickMoveComponent"]["rightClick"] = { std::get<0>(clickMoveComponent->_rightClick), std::get<1>(clickMoveComponent->_rightClick) };
		save->_jsonFile["Entity"]["ClickMoveComponent"]["speed"] = clickMoveComponent->_speed;
		save->_jsonFile["Entity"]["ClickMoveComponent"]["move"] = clickMoveComponent->_move;
	}

	void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		std::shared_ptr<ClickMoveComponent> clickMoveComponent = targetEntity->getComponent<ClickMoveComponent>();
		if (!clickMoveComponent) {
			clickMoveComponent = std::shared_ptr<ClickMoveComponent>(new ClickMoveComponent());
		}

		if (jsonFile["ClickMoveComponent"].empty())
			return;

		clickMoveComponent->_leftClick = { jsonFile["ClickMoveComponent"]["leftClick"][0], jsonFile["ClickMoveComponent"]["leftClick"][1] };
		clickMoveComponent->_rightClick = { jsonFile["ClickMoveComponent"]["rightClick"][0], jsonFile["ClickMoveComponent"]["rightClick"][1] };
		clickMoveComponent->_move = jsonFile["ClickMoveComponent"]["move"];
		clickMoveComponent->initialize(jsonFile["ClickMoveComponent"]["speed"]);
		targetEntity->addComponent(clickMoveComponent);
	}

};

class TextureComponentFunctor : public ComponentFunctor {
public:
	//TODO: How to use atlas and textures dynamically
	std::shared_ptr<Component> createFunctor() {
		Shader shader;
		auto program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
		int textureID;
		std::cout << "Enter texture ID" << std::endl;
		std::cin >> textureID;
		GLuint programID;
		std::cout << "Enter program ID" << std::endl;
		std::cin >> programID;

		auto targetTexture = TextureManager::instance()->getTexture(textureID);
		if (targetTexture->getRow() > 1 || targetTexture->getColumn() > 1) {
			std::shared_ptr<AnimatedTextureComponent> textureComponent(new AnimatedTextureComponent());
			int tilesCount = targetTexture->getRow() * targetTexture->getColumn();
			std::cout << "Enter order for " << tilesCount << " tiles" << std::endl;
			std::vector<int> tilesOrder;
			while (tilesCount-- > 0) {
				int input;
				std::cin >> input;
				tilesOrder.push_back(input);
			}
			
			std::cout << "Enter latency for " << tilesCount << " tiles" << std::endl;
			std::vector<int> tilesLatency;
			while (tilesCount-- > 0) {
				int input;
				std::cin >> input;
				tilesLatency.push_back(input);
			}

			textureComponent->initialize(targetTexture, tilesOrder, tilesLatency, programID);
			return textureComponent;
		}
	
		std::shared_ptr<TextureComponent> textureComponent(new TextureComponent());
		textureComponent->initialize(targetTexture, programID);
		return textureComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<TextureComponent>();
	}

	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<TextureComponent> textureComponent = targetEntity->getComponent<TextureComponent>();
		if (!textureComponent)
			return;
		save->_jsonFile["Entity"]["TextureComponent"]["textureID"] = textureComponent->_texture->getTextureID();
	}

	void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		if (!objectComponent)
			return;
		auto program = objectComponent->_program;
		std::shared_ptr<TextureComponent> textureComponent = targetEntity->getComponent<TextureComponent>();
		if (!textureComponent) {
			textureComponent = std::shared_ptr<TextureComponent>(new TextureComponent());
			targetEntity->addComponent(textureComponent);
		}
		int textureID = jsonFile["TextureComponent"]["textureID"];

		textureComponent->initialize(textureID, program);
	}

};

class ObjectComponentFunctor : public ComponentFunctor {
	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		if (!objectComponent)
			return;

		save->_jsonFile["Entity"]["ID"] = entityID;
		save->_jsonFile["Entity"]["ObjectComponent"]["sceneCoord"] = {objectComponent->_sceneX, objectComponent->_sceneY};
		save->_jsonFile["Entity"]["ObjectComponent"]["objectSize"] = { objectComponent->_objectWidth, objectComponent->_objectHeight };
	}

	void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		if (!objectComponent) {
			objectComponent = std::shared_ptr<ObjectComponent>(new ObjectComponent());
			targetEntity->addComponent(objectComponent);
		}

		Shader shader;
		auto program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
		float sceneX = jsonFile["ObjectComponent"]["sceneCoord"][0];
		float sceneY = jsonFile["ObjectComponent"]["sceneCoord"][1];
		float objectWidth = jsonFile["ObjectComponent"]["objectSize"][0];
		float objectHeight = jsonFile["ObjectComponent"]["objectSize"][1];
		objectComponent->initialize(sceneX, sceneY, objectWidth, objectHeight, program);
	}

};

void registerComponentFunctors();