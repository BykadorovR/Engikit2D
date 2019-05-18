#pragma once
#include "Component.h"
#include "UIComponent.h"
#include "GraphicComponent.h"
#include "Texture.h"

class ComponentFunctor {
public:
	virtual std::shared_ptr<Component> createFunctor() = 0;
	virtual void removeFunctor(std::shared_ptr<Entity> targetEntity) = 0;
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
	};
	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<ClickMoveComponent>();
	};

};

class TextureComponentFunctor : public ComponentFunctor {
public:
	TextureComponentFunctor(std::shared_ptr<TextureManager> textureManager) {
		_textureManager = textureManager;
	}

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

		std::shared_ptr<Texture> targetTexture = _textureManager->getTexture(textureID);
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
	};

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<TextureComponent>();
	};

	std::shared_ptr<TextureManager> _textureManager;

};

void registerComponentFunctors(std::shared_ptr<TextureManager> textureManager);