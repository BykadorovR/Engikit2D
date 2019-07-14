#pragma once
#include "Component.h"
#include "UIComponent.h"
#include "GraphicComponent.h"
#include "Texture.h"
#include "Camera.h"
#include <sstream>

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
			std::vector<int> tilesOrder, tilesLatency;
			std::string line;

			while (tilesOrder.size() == 0 || tilesOrder.size() != tilesLatency.size()) {
				tilesOrder.clear();
				tilesLatency.clear();
				std::cout << "Enter order for tiles (separated by spaces, enter -1 in the end)" << std::endl;
				int input;
				while ((cin >> input) && input != -1)
					tilesOrder.push_back(input);

				std::cout << "Enter latency for tiles (separated by spaces, enter -1 in the end)" << std::endl;
				while ((cin >> input) && input != -1)
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
		if (textureComponent)
			save->_jsonFile["Entity"]["TextureComponent"]["textureID"] = textureComponent->_texture->getTextureID();

		std::shared_ptr<AnimatedTextureComponent> animatedTextureComponent = targetEntity->getComponent<AnimatedTextureComponent>();
		if (animatedTextureComponent) {
			save->_jsonFile["Entity"]["AnimatedTextureComponent"]["textureID"] = animatedTextureComponent->_texture->getTextureID();
			save->_jsonFile["Entity"]["AnimatedTextureComponent"]["tilesOrder"] = animatedTextureComponent->_tilesOrder;
			save->_jsonFile["Entity"]["AnimatedTextureComponent"]["tilesLatency"] = animatedTextureComponent->_tilesLatency;
		}
	}

	void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		if (!objectComponent)
			return;
		auto program = objectComponent->_program;
		if (!jsonFile["TextureComponent"].empty()) {
			std::shared_ptr<TextureComponent> textureComponent = targetEntity->getComponent<TextureComponent>();
			if (!textureComponent) {
				textureComponent = std::shared_ptr<TextureComponent>(new TextureComponent());
				targetEntity->addComponent(textureComponent);
			}
			int textureID = jsonFile["TextureComponent"]["textureID"];

			textureComponent->initialize(textureID, program);
		}
		if (!jsonFile["AnimatedTextureComponent"].empty()) {
			std::shared_ptr<AnimatedTextureComponent> animatedTextureComponent = targetEntity->getComponent<AnimatedTextureComponent>();
			if (!animatedTextureComponent) {
				animatedTextureComponent = std::shared_ptr<AnimatedTextureComponent>(new AnimatedTextureComponent());
				targetEntity->addComponent(animatedTextureComponent);
			}
			int textureID = jsonFile["AnimatedTextureComponent"]["textureID"];
			std::vector<int> tilesOrder = jsonFile["AnimatedTextureComponent"]["tilesOrder"];
			std::vector<int> tilesLatency = jsonFile["AnimatedTextureComponent"]["tilesLatency"];

			animatedTextureComponent->initialize(textureID, tilesOrder, tilesLatency, program);
		}
	}

};

class ObjectComponentFunctor : public ComponentFunctor {
	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor() {
		std::shared_ptr<ObjectComponent> objectComponent(new ObjectComponent());
		int sceneX, sceneY, objectWidth, objectHeight;
		GLuint program;
		std::cout << "Enter x and y of object on scene:" << std::endl;
		std::cin >> sceneX >> sceneY;
		std::cout << "Enter width and height of object:" << std::endl;
		std::cin >> objectWidth >> objectHeight;
		std::cout << "Enter programID (use the same as in current object):" << std::endl;
		std::cin >> program;
		objectComponent->initialize(sceneX, sceneY, objectWidth, objectHeight, program);

		return objectComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<ObjectComponent>();
	}
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

class ClickInsideFunctor : public ComponentFunctor {
	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor() {
		std::shared_ptr<ClickInsideComponent> clickComponent(new ClickInsideComponent());
		bool moveToByClick;
		std::cout << "Move toward this object allowed?" << std::endl;
		std::cin >> moveToByClick;
		clickComponent->initialize(moveToByClick);
		return clickComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<ClickInsideComponent>();
	}
	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<ClickInsideComponent> clickInsideComponent = targetEntity->getComponent<ClickInsideComponent>();
		if (!clickInsideComponent)
			return;

		save->_jsonFile["Entity"]["ID"] = entityID;
		save->_jsonFile["Entity"]["ClickInsideComponent"]["moveToByClick"] = clickInsideComponent->_moveToByClick;
	}

	void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		if (jsonFile["ClickInsideComponent"].empty())
			return;

		std::shared_ptr<ClickInsideComponent> clickInsideComponent = targetEntity->getComponent<ClickInsideComponent>();
		if (!clickInsideComponent) {
			clickInsideComponent = std::shared_ptr<ClickInsideComponent>(new ClickInsideComponent());
			targetEntity->addComponent(clickInsideComponent);
		}

		bool moveToByClick = jsonFile["ClickInsideComponent"]["moveToByClick"];
		clickInsideComponent->initialize(moveToByClick);
	}
};

class GroupEntitiesFunctor : public ComponentFunctor {
	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor() {
		std::shared_ptr<GroupEntitiesComponent> groupEntitiesComponent(new GroupEntitiesComponent());
		uint32_t groupNumber;
		std::string groupName;
		std::cout << "Group number" << std::endl;
		std::cin >> groupNumber;
		std::cout << "Group name" << std::endl;
		std::cin >> groupName;
		groupEntitiesComponent->initialize(groupNumber, groupName);
		return groupEntitiesComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<GroupEntitiesComponent>();
	}
	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<GroupEntitiesComponent> groupEntitiesComponent = targetEntity->getComponent<GroupEntitiesComponent>();
		if (!groupEntitiesComponent)
			return;

		save->_jsonFile["Entity"]["ID"] = entityID;
		save->_jsonFile["Entity"]["GroupEntitiesComponent"]["groupNumber"] = groupEntitiesComponent->_groupNumber;
		save->_jsonFile["Entity"]["GroupEntitiesComponent"]["groupName"] = groupEntitiesComponent->_groupName;
	}

	void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		if (jsonFile["GroupEntitiesComponent"].empty())
			return;

		std::shared_ptr<GroupEntitiesComponent> groupEntitiesComponent = targetEntity->getComponent<GroupEntitiesComponent>();
		if (!groupEntitiesComponent) {
			groupEntitiesComponent = std::shared_ptr<GroupEntitiesComponent>(new GroupEntitiesComponent());
			targetEntity->addComponent(groupEntitiesComponent);
		}

		uint32_t groupNumber = jsonFile["GroupEntitiesComponent"]["groupNumber"];
		std::string groupName = jsonFile["GroupEntitiesComponent"]["groupName"];
		groupEntitiesComponent->initialize(groupNumber, groupName);
	}
};

class InteractionAddToEntityFunctor : public ComponentFunctor {
	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor() {
		std::shared_ptr<InteractionAddToEntityComponent> interactionAddToEntityComponent(new InteractionAddToEntityComponent());
		bool interactionMember;
		std::cout << "Object = 0, Subject = 1" << std::endl;
		std::cin >> interactionMember;
		interactionAddToEntityComponent->initialize((InteractionMember) interactionMember);
		return interactionAddToEntityComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<InteractionAddToEntityComponent>();
	}
	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<InteractionAddToEntityComponent> interactionAddToEntityComponent = targetEntity->getComponent<InteractionAddToEntityComponent>();
		if (!interactionAddToEntityComponent)
			return;

		save->_jsonFile["Entity"]["ID"] = entityID;
		save->_jsonFile["Entity"]["InteractionAddToEntityComponent"]["interactMember"] = interactionAddToEntityComponent->_interactionMember;
	}

	void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		if (jsonFile["InteractionAddToEntityComponent"].empty())
			return;

		std::shared_ptr<InteractionAddToEntityComponent> interactionAddToEntityComponent = targetEntity->getComponent<InteractionAddToEntityComponent>();
		if (!interactionAddToEntityComponent) {
			interactionAddToEntityComponent = std::shared_ptr<InteractionAddToEntityComponent>(new InteractionAddToEntityComponent());
			targetEntity->addComponent(interactionAddToEntityComponent);
		}

		InteractionMember interactionMember = jsonFile["InteractionAddToEntityComponent"]["interactMember"];
		interactionAddToEntityComponent->initialize(interactionMember);
	}
};

class TransformFunctor : public ComponentFunctor {
	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor() {
		std::shared_ptr<TransformComponent> transformComponent(new TransformComponent());
		std::tuple<float, float> coords;
		GLuint program;
		std::cout << "Enter programID:" << std::endl;
		std::cin >> program;
		std::cout << "Enter direction (x,y):" << std::endl;
		std::cin >> std::get<0>(coords) >> std::get<1>(coords);
		
		transformComponent->initialize(program);
		transformComponent->setTransform(coords);
		return transformComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<TransformComponent>();
	}
	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<TransformComponent> transformComponent = targetEntity->getComponent<TransformComponent>();
		if (!transformComponent)
			return;

		save->_jsonFile["Entity"]["ID"] = entityID;
		save->_jsonFile["Entity"]["TransformComponent"]["coords"] = transformComponent->_coords;
	}

	void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		if (jsonFile["TransformComponent"].empty())
			return;

		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		if (!objectComponent)
			return;
		auto program = objectComponent->_program;

		std::shared_ptr<TransformComponent> transformComponent = targetEntity->getComponent<TransformComponent>();
		if (!transformComponent) {
			transformComponent = std::shared_ptr<TransformComponent>(new TransformComponent());
			targetEntity->addComponent(transformComponent);
		}

		std::tuple<float, float> coords = jsonFile["TransformComponent"]["coords"];
		transformComponent->initialize(program);
		transformComponent->setTransform(coords);
	}
};

class CameraFunctor : public ComponentFunctor {
	std::shared_ptr<Component> createFunctor() {
		std::shared_ptr<CameraComponent> cameraComponent(new CameraComponent());
		int entityID;
		std::cout << "Enter entityID:" << std::endl;
		std::cin >> entityID;
		
		cameraComponent->initialize(entityID);
		return cameraComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<CameraComponent>();
	}

	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {

	}

	void deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {

	}
};

void registerComponentFunctors();