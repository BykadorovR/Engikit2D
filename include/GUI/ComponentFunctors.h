#pragma once
#include "Component.h"
#include "UIComponent.h"
#include "GraphicComponent.h"
#include "Texture.h"
#include "Camera.h"
#include <sstream>
#include "TextLoader.h"
#include <string>
#include "TextHelper.h"

class ComponentFunctor : public ITextEvent {
public:
	virtual void configureFunctor(std::shared_ptr<Entity> targetEntity) = 0;
	virtual std::shared_ptr<Component> createFunctor(std::shared_ptr<Entity> targetEntity) = 0;
	virtual void removeFunctor(std::shared_ptr<Entity> targetEntity) = 0;
	virtual void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) = 0;
	virtual int deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) = 0;
};

extern std::map<std::string, std::shared_ptr<ComponentFunctor> > componentFunctors;

class TextureComponentFunctor : public ComponentFunctor {
public:
	void configureFunctor(std::shared_ptr<Entity> targetEntity) {

	}
	//TODO: How to use atlas and textures dynamically
	std::shared_ptr<Component> createFunctor(std::shared_ptr<Entity> targetEntity) {
		std::shared_ptr<TextComponent> textComponent = targetEntity->getComponent<TextComponent>();
		int textureID;
		std::cout << "Enter texture ID" << std::endl;
		std::cin >> textureID;
		GLuint programID;
		if (textComponent) {
			targetEntity->removeComponent<TextComponent>();
			std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
			Shader shader;
			GLuint program;
			program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
			objectComponent->initialize(objectComponent->_sceneX, objectComponent->_sceneY, objectComponent->_objectWidth, objectComponent->_objectHeight, program);
			programID = program;
		}
		else {
			std::cout << "Enter program ID" << std::endl;
			std::cin >> programID;
		}
		
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
		std::shared_ptr<TextureComponent> textureComponent = targetEntity->getComponent<TextureComponent>();
		if (textureComponent) {
			textureComponent->_solid = 1;
			textureComponent->_texture = nullptr;
		}

		std::shared_ptr<AnimatedTextureComponent> animatedTextureComponent = targetEntity->getComponent<AnimatedTextureComponent>();
		if (animatedTextureComponent) {
			int program = animatedTextureComponent->_program;
			targetEntity->removeComponent<AnimatedTextureComponent>();
			std::shared_ptr<TextureComponent> textureComponent(new TextureComponent());
			textureComponent->initialize(program);
			targetEntity->addComponent(textureComponent);
		}
	}

	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<TextureComponent> textureComponent = targetEntity->getComponent<TextureComponent>();
		if (textureComponent && textureComponent->_texture != nullptr) {
			save->_jsonFile["Entity"][std::to_string(entityID)]["TextureComponent"]["textureID"] = textureComponent->_texture->getTextureID();
		}

		std::shared_ptr<AnimatedTextureComponent> animatedTextureComponent = targetEntity->getComponent<AnimatedTextureComponent>();
		if (animatedTextureComponent) {
			save->_jsonFile["Entity"][std::to_string(entityID)]["AnimatedTextureComponent"]["textureID"] = animatedTextureComponent->_texture->getTextureID();
			save->_jsonFile["Entity"][std::to_string(entityID)]["AnimatedTextureComponent"]["tilesOrder"] = animatedTextureComponent->_tilesOrder;
			save->_jsonFile["Entity"][std::to_string(entityID)]["AnimatedTextureComponent"]["tilesLatency"] = animatedTextureComponent->_tilesLatency;
		}
	}

	int deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		if (!objectComponent)
			return 1;

		auto program = objectComponent->_program;
		if (!jsonFile["TextureComponent"].empty()) {
			std::shared_ptr<TextureComponent> textureComponent = targetEntity->getComponent<TextureComponent>();
			if (!textureComponent) {
				textureComponent = std::shared_ptr<TextureComponent>(new TextureComponent());
				targetEntity->addComponent(textureComponent);
			}
			int textureID = jsonFile["TextureComponent"]["textureID"];

			textureComponent->initialize(textureID, program);
		} else if (!jsonFile["AnimatedTextureComponent"].empty()) {
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
		else if (jsonFile["TextComponent"].empty())
		{
			std::shared_ptr<TextureComponent> textureComponent = targetEntity->getComponent<TextureComponent>();
			if (!textureComponent) {
				textureComponent = std::shared_ptr<TextureComponent>(new TextureComponent());
				targetEntity->addComponent(textureComponent);
			}
			textureComponent->initialize(program);
		}
		return 0;
	}
};

class ObjectComponentFunctor : public ComponentFunctor {
	void configureFunctor(std::shared_ptr<Entity> targetEntity) {
		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		TextHelper helper;
		std::shared_ptr<TextCallback> callback = std::make_shared<TextCallback>();
		callback->setValue(&objectComponent->_sceneX, TextConversion::MY_FLOAT);
		helper.getValue(callback, &world);
	}

	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor(std::shared_ptr<Entity> targetEntity) {
		std::shared_ptr<ObjectComponent> objectComponent(new ObjectComponent());
		TextHelper helper;
		TextCallback callback;
		callback.setValue(&objectComponent->_sceneX, TextConversion::MY_FLOAT);
		/*
		int sceneX, sceneY, objectWidth, objectHeight;
		GLuint program;
		std::cout << "Enter x and y of object on scene:" << std::endl;
		std::cin >> sceneX >> sceneY;
		std::cout << "Enter width and height of object:" << std::endl;
		std::cin >> objectWidth >> objectHeight;
		std::cout << "Enter programID (use the same as in current object):" << std::endl;
		std::cin >> program;
		float speed;
		std::cout << "Speed coef from camera speed for this object" << std::endl;
		std::cin >> speed;
		objectComponent->initialize(sceneX, sceneY, objectWidth, objectHeight, program);
		objectComponent->_cameraCoefSpeed = speed;
		*/
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

		save->_jsonFile["Entity"][std::to_string(entityID)]["ObjectComponent"]["cameraCoefSpeed"] = objectComponent->_cameraCoefSpeed;
		save->_jsonFile["Entity"][std::to_string(entityID)]["ObjectComponent"]["sceneCoord"] = {objectComponent->_sceneX, objectComponent->_sceneY};
		save->_jsonFile["Entity"][std::to_string(entityID)]["ObjectComponent"]["objectSize"] = { objectComponent->_objectWidth, objectComponent->_objectHeight };
	}

	int deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		if (!objectComponent) {
			objectComponent = std::shared_ptr<ObjectComponent>(new ObjectComponent());
			targetEntity->addComponent(objectComponent);
		}

		float cameraSpeed = jsonFile["ObjectComponent"]["cameraCoefSpeed"];
		float sceneX = jsonFile["ObjectComponent"]["sceneCoord"][0];
		float sceneY = jsonFile["ObjectComponent"]["sceneCoord"][1];
		float objectWidth = jsonFile["ObjectComponent"]["objectSize"][0];
		float objectHeight = jsonFile["ObjectComponent"]["objectSize"][1];
		objectComponent->_cameraCoefSpeed = cameraSpeed;
		Shader shader;
		GLuint program;
		if (jsonFile["TextComponent"].empty()) {
			program = shader.buildProgramFromAsset("../data/shaders/shader.vsh", "../data/shaders/shader.fsh");
			objectComponent->initialize(sceneX, sceneY, objectWidth, objectHeight, program);
		}
		else {
			program = shader.buildProgramFromAsset("../data/shaders/text.vsh", "../data/shaders/text.fsh");
			objectComponent->initializeText(sceneX, sceneY, objectWidth, objectHeight, program);
		}

		return 0;
	}
};

class TextComponentFunctor : public ComponentFunctor {
	void configureFunctor(std::shared_ptr<Entity> targetEntity) {

	}
	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor(std::shared_ptr<Entity> targetEntity) {
		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		Shader shader;
		GLuint program;
		program = shader.buildProgramFromAsset("../data/shaders/text.vsh", "../data/shaders/text.fsh");
		objectComponent->_program = program;

		std::shared_ptr<TextureComponent> textureComponent = targetEntity->getComponent<TextureComponent>();
		if (textureComponent)
			targetEntity->removeComponent<TextureComponent>();

		std::shared_ptr<AnimatedTextureComponent> animatedTextureComponent = targetEntity->getComponent<AnimatedTextureComponent>();
		if (animatedTextureComponent)
			targetEntity->removeComponent<AnimatedTextureComponent>();

		std::shared_ptr<TextComponent> textComponent(new TextComponent());
		std::shared_ptr<TextLoader> textLoader = std::make_shared<TextLoader>();
		textLoader->bufferSymbols(48);
		std::string text;
		std::cout << "Enter text to display:" << std::endl;
		cin.ignore();
		std::getline(cin, text);
		float scale;
		std::cout << "Enter scale:" << std::endl;
		std::cin >> scale;
		std::vector<float> color;
		color.resize(3);
		std::cout << "Enter color (r, g, b):" << std::endl;
		std::cin >> color[0] >> color[1] >> color[2];
		int type;
		std::cout << "Enter type: label(0), edit text(1)" << std::endl;
		std::cin >> type;

		textComponent->initialize(textLoader, text, scale, color, (TextType) type);
		return textComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<TextComponent>();
	}
	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<TextComponent> textComponent = targetEntity->getComponent<TextComponent>();
		if (!textComponent)
			return;

		save->_jsonFile["Entity"][std::to_string(entityID)]["TextComponent"]["text"] = textComponent->_text;
		save->_jsonFile["Entity"][std::to_string(entityID)]["TextComponent"]["scale"] = textComponent->_scale;
		save->_jsonFile["Entity"][std::to_string(entityID)]["TextComponent"]["color"] = textComponent->_color;
		save->_jsonFile["Entity"][std::to_string(entityID)]["TextComponent"]["type"] = textComponent->_type;
		
	}

	int deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		if (jsonFile["TextComponent"].empty())
			return -1;

		int entityID = targetEntity->_index;
		std::shared_ptr<TextComponent> textComponent = targetEntity->getComponent<TextComponent>();
		if (!textComponent) {
			textComponent = std::shared_ptr<TextComponent>(new TextComponent());
			targetEntity->addComponent(textComponent);
		}

		std::shared_ptr<TextLoader> loader = std::make_shared<TextLoader>();
		loader->bufferSymbols(48);
		std::string text = jsonFile["TextComponent"]["text"];
		float scale = jsonFile["TextComponent"]["scale"];
		std::vector<float> color = jsonFile["TextComponent"]["color"];
		int type = jsonFile["TextComponent"]["type"];
		textComponent->initialize(loader, text, scale, color, (TextType) type);
		return 0;
	}
};

class ClickInsideFunctor : public ComponentFunctor {
	void configureFunctor(std::shared_ptr<Entity> targetEntity) {

	}
	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor(std::shared_ptr<Entity> targetEntity) {
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

		save->_jsonFile["Entity"][std::to_string(entityID)]["ClickInsideComponent"]["moveToByClick"] = clickInsideComponent->_moveToByClick;
	}

	int deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		if (jsonFile["ClickInsideComponent"].empty())
			return -1;

		std::shared_ptr<ClickInsideComponent> clickInsideComponent = targetEntity->getComponent<ClickInsideComponent>();
		if (!clickInsideComponent) {
			clickInsideComponent = std::shared_ptr<ClickInsideComponent>(new ClickInsideComponent());
			targetEntity->addComponent(clickInsideComponent);
		}

		bool moveToByClick = jsonFile["ClickInsideComponent"]["moveToByClick"];
		clickInsideComponent->initialize(moveToByClick);
		return 0;
	}
};

class GroupEntitiesFunctor : public ComponentFunctor {
	void configureFunctor(std::shared_ptr<Entity> targetEntity) {

	}
	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor(std::shared_ptr<Entity> targetEntity) {
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

		save->_jsonFile["Entity"][std::to_string(entityID)]["GroupEntitiesComponent"]["groupNumber"] = groupEntitiesComponent->_groupNumber;
		save->_jsonFile["Entity"][std::to_string(entityID)]["GroupEntitiesComponent"]["groupName"] = groupEntitiesComponent->_groupName;
	}

	int deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		if (jsonFile["GroupEntitiesComponent"].empty())
			return -1;

		std::shared_ptr<GroupEntitiesComponent> groupEntitiesComponent = targetEntity->getComponent<GroupEntitiesComponent>();
		if (!groupEntitiesComponent) {
			groupEntitiesComponent = std::shared_ptr<GroupEntitiesComponent>(new GroupEntitiesComponent());
			targetEntity->addComponent(groupEntitiesComponent);
		}

		uint32_t groupNumber = jsonFile["GroupEntitiesComponent"]["groupNumber"];
		std::string groupName = jsonFile["GroupEntitiesComponent"]["groupName"];
		groupEntitiesComponent->initialize(groupNumber, groupName);
		return 0;
	}
};

class InteractionAddToEntityFunctor : public ComponentFunctor {
	void configureFunctor(std::shared_ptr<Entity> targetEntity) {

	}
	//this component can't be added to Entity, so it's just a stub
	std::shared_ptr<Component> createFunctor(std::shared_ptr<Entity> targetEntity) {
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

		save->_jsonFile["Entity"][std::to_string(entityID)]["InteractionAddToEntityComponent"]["interactMember"] = interactionAddToEntityComponent->_interactionMember;
	}

	int deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		if (jsonFile["InteractionAddToEntityComponent"].empty())
			return -1;

		std::shared_ptr<InteractionAddToEntityComponent> interactionAddToEntityComponent = targetEntity->getComponent<InteractionAddToEntityComponent>();
		if (!interactionAddToEntityComponent) {
			interactionAddToEntityComponent = std::shared_ptr<InteractionAddToEntityComponent>(new InteractionAddToEntityComponent());
			targetEntity->addComponent(interactionAddToEntityComponent);
		}

		InteractionMember interactionMember = jsonFile["InteractionAddToEntityComponent"]["interactMember"];
		interactionAddToEntityComponent->initialize(interactionMember);
		return 0;
	}
};

class MoveFunctor : public ComponentFunctor {
	void configureFunctor(std::shared_ptr<Entity> targetEntity) {

	}
	std::shared_ptr<Component> createFunctor(std::shared_ptr<Entity> targetEntity) {
		std::shared_ptr<MoveComponent> moveComponent(new MoveComponent());
		MoveTypes moveType;
		int moveTypeInt;
		std::cout << "Choose number of type:" << std::endl;
		std::cout << "PlayerControlled " << PlayerControlled << std::endl;
		std::cout << "Predefined " << StaticallyDefined << std::endl;
		std::cin >> moveTypeInt;
		moveType = (MoveTypes) moveTypeInt;
		GLuint program;
		std::cout << "Enter programID:" << std::endl;
		std::cin >> program;
		int speed;
		std::cout << "Enter the speed" << std::endl;
		std::cin >> speed;

		switch (moveType) {
			case PlayerControlled:
				moveComponent->initialize(moveType, program, speed);
			break;
			case StaticallyDefined:
				std::tuple<float, float> endPoint;
				std::cout << "Enter end point (x,y):" << std::endl;
				std::cin >> std::get<0>(endPoint) >> std::get<1>(endPoint);
				moveComponent->initialize(moveType, program, speed, endPoint);
			break;
		}

		return moveComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<MoveComponent>();
	}

	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<MoveComponent> moveComponent = targetEntity->getComponent<MoveComponent>();
		if (!moveComponent)
			return;

		save->_jsonFile["Entity"][std::to_string(entityID)]["MoveComponent"]["type"] = moveComponent->_type;
		if (moveComponent->_type == StaticallyDefined)
			save->_jsonFile["Entity"][std::to_string(entityID)]["MoveComponent"]["coords"] = moveComponent->_coords;
		else {
			save->_jsonFile["Entity"][std::to_string(entityID)]["MoveComponent"]["leftClick"] = { std::get<0>(moveComponent->_leftClick), std::get<1>(moveComponent->_leftClick) };
			save->_jsonFile["Entity"][std::to_string(entityID)]["MoveComponent"]["rightClick"] = { std::get<0>(moveComponent->_rightClick), std::get<1>(moveComponent->_rightClick) };
		}
		save->_jsonFile["Entity"][std::to_string(entityID)]["MoveComponent"]["speed"] = moveComponent->_speed;
		save->_jsonFile["Entity"][std::to_string(entityID)]["MoveComponent"]["move"] = moveComponent->_move;
	}

	int deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		if (jsonFile["MoveComponent"].empty())
			return -1;

		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		if (!objectComponent)
			return 1;
		auto program = objectComponent->_program;

		std::shared_ptr<MoveComponent> moveComponent = targetEntity->getComponent<MoveComponent>();
		if (!moveComponent) {
			moveComponent = std::shared_ptr<MoveComponent>(new MoveComponent());
			targetEntity->addComponent(moveComponent);
		}

		MoveTypes type = jsonFile["MoveComponent"]["type"];
		int speed = jsonFile["MoveComponent"]["speed"];
		bool move = jsonFile["MoveComponent"]["move"];
		if (type == StaticallyDefined) {
			std::tuple<float, float> coords = jsonFile["MoveComponent"]["coords"];
			moveComponent->initialize(type, program, speed, coords);
		}
		else {
			std::tuple<float, float> leftClick = jsonFile["MoveComponent"]["leftClick"];
			std::tuple<float, float> rightClick = jsonFile["MoveComponent"]["rightClick"];
			moveComponent->initialize(type, program, speed);
			moveComponent->_leftClick = leftClick;
			moveComponent->_rightClick = rightClick;
			moveComponent->_move = move;
		}
		return 0;
	}
};

class CameraFunctor : public ComponentFunctor {
	void configureFunctor(std::shared_ptr<Entity> targetEntity) {

	}
	std::shared_ptr<Component> createFunctor(std::shared_ptr<Entity> targetEntity) {
		std::shared_ptr<CameraComponent> cameraComponent(new CameraComponent());
		int entityID;
		std::cout << "Enter entityID:" << std::endl;
		std::cin >> entityID;
		int programID;
		std::cout << "Enter programID:" << std::endl;
		std::cin >> programID;
		int speed;
		std::cout << "Enter camera speed: " << std::endl;
		std::cin >> speed;

		cameraComponent->initialize(entityID, speed, programID);
		return cameraComponent;
	}

	void removeFunctor(std::shared_ptr<Entity> targetEntity) {
		targetEntity->removeComponent<CameraComponent>();
	}

	void serializeFunctor(std::shared_ptr<Entity> targetEntity, std::shared_ptr<GUISave> save) {
		int entityID = targetEntity->_index;
		std::shared_ptr<CameraComponent> cameraComponent = targetEntity->getComponent<CameraComponent>();
		if (!cameraComponent)
			return;

		save->_jsonFile["Entity"][std::to_string(entityID)]["CameraComponent"]["leftClick"] = { std::get<0>(cameraComponent->_leftClick), std::get<1>(cameraComponent->_leftClick) };
		save->_jsonFile["Entity"][std::to_string(entityID)]["CameraComponent"]["rightClick"] = { std::get<0>(cameraComponent->_rightClick), std::get<1>(cameraComponent->_rightClick) };
		save->_jsonFile["Entity"][std::to_string(entityID)]["CameraComponent"]["speed"] = cameraComponent->_cameraSpeed;
		save->_jsonFile["Entity"][std::to_string(entityID)]["CameraComponent"]["move"] = cameraComponent->_move;
	}

	int deserializeFunctor(std::shared_ptr<Entity> targetEntity, json jsonFile) {
		int entityID = targetEntity->_index;
		if (jsonFile["CameraComponent"].empty())
			return -1;

		std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
		if (!objectComponent)
			return 1;
		auto program = objectComponent->_program;

		std::shared_ptr<CameraComponent> cameraComponent = targetEntity->getComponent<CameraComponent>();
		if (!cameraComponent) {
			cameraComponent = std::shared_ptr<CameraComponent>(new CameraComponent());
			targetEntity->addComponent(cameraComponent);
		}

		int speed = jsonFile["CameraComponent"]["speed"];
		bool move = jsonFile["CameraComponent"]["move"];
		std::tuple<float, float> leftClick = jsonFile["CameraComponent"]["leftClick"];
		std::tuple<float, float> rightClick = jsonFile["CameraComponent"]["rightClick"];
		cameraComponent->initialize(entityID, speed, program);
		cameraComponent->_leftClick = leftClick;
		cameraComponent->_rightClick = rightClick;
		cameraComponent->_move = move;
		
		return 0;
	}
};

void registerComponentFunctors();