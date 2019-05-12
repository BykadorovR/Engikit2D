#pragma once
#include "Component.h"
#include "UIComponent.h"
#include "GraphicComponent.h"

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
	//TODO: How to use atlas and textures dynamically
	std::shared_ptr<Component> createFunctor() {
		std::shared_ptr<TextureComponent> textureComponent(new TextureComponent());
		Shader shader;
		auto program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
		std::string texturePath;
		std::cout << "Enter path to texture" << std::endl;
		std::cin >> texturePath;
		Texture textureRaw(texturePath, 0, 0);
		std::shared_ptr<TextureAtlas> atlas = std::make_shared<TextureAtlas>(textureRaw.getWidth(), textureRaw.getHeight());
		textureRaw.setAtlas(atlas);
		atlas->loadAtlas();
		textureComponent->initialize(textureRaw, program);
		return textureComponent;
	};

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<TextureComponent>();
	};

};

void registerComponentFunctors();