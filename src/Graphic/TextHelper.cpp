#include "GraphicComponent.h"
#include "UIComponent.h"
#include "TextHelper.h"
#include "ComponentFunctors.h"

void TextCallback::setValue(float* valueF) {
	_valueF = valueF;
	_conversion = MY_FLOAT;
}

void TextCallback::setValue(int* valueI) {
	_valueI = valueI;
	_conversion = MY_INT;
}

void TextCallback::setValue(std::string* valueS) {
	_valueS = valueS;
	_conversion = MY_STRING;
}

void TextCallback::setValue(std::vector<int>* valueT) {
	int* temp = valueT->data();
	_valueT = &temp;
}

void TextCallback::callToSet(std::string value) {
	switch (_conversion) {
	case MY_FLOAT:
		*_valueF = std::stof(value);
		break;
	case MY_INT:
		*_valueI = std::stoi(value);
		break;
	case MY_STRING:
		*_valueS = value;
		break;
	case MY_TUPLE:;

		std::istringstream is(value);
		int n;
		int size = 0;
		while (is >> n) {
			size++;
		}

		int* temp = new int[size];
		std::istringstream is1(value);
		int i = 0;
		while (is1 >> n) {
			temp[i++] = n;
		}

		_valueT = &temp;
	}
}

void TextHelper::getValue(std::shared_ptr<TextCallback> callback, std::string text, int x, int y, int width, int height, float scale) {
	shared_ptr<Entity> sprite = nullptr;
	for (auto &elem : _buffer) {
		if (world.isEntityAttached(elem) == false) {
			sprite = elem;
			break;
		}
	}
	if (!sprite) {
		sprite = createText("Enter", x, y, width, height, scale, true);
	}

	std::shared_ptr<TextComponent> textComponent = sprite->getComponent<TextComponent>();
	textComponent->_text = text;
	textComponent->_textBack = text;
	textComponent->_type = TextType::EDIT;
	textComponent->_scale = scale;
	textComponent->setCallback(callback);
	std::shared_ptr<ObjectComponent> objectComponent = sprite->getComponent<ObjectComponent>();
	objectComponent->_sceneX = x;
	objectComponent->_sceneY = y;
	objectComponent->_objectWidth = width;
	objectComponent->_objectHeight = height;

	TextHelper::instance()->attachText(sprite);
}

TextHelper* TextHelper::instance() {
	static TextHelper helper;
	return &helper;
}

TextHelper::TextHelper() {
	//need to create some amount of text objects to use
	for (int i = 0; i < _bufferSize; i++) {
		auto sprite = createText("", 0, 0, 200, 50, 0.4, false);
	}
}

void TextHelper::attachText(std::shared_ptr<Entity> entity) {
	world.registerEntity(entity);
}

bool TextHelper::detachText(std::shared_ptr<Entity> entity) {
	//Need to clear components to default
	std::shared_ptr<TextComponent> textComponent = entity->getComponent<TextComponent>();
	if (textComponent)
		textComponent->_focus = false;
	return world.unregisterEntity(entity);
}

shared_ptr<Entity> TextHelper::createText(std::string text, int x, int y, int width, int height, float scale, bool edit) {
	shared_ptr<Entity> sprite = nullptr;
	for (auto &elem : _buffer) {
		if (world.isEntityAttached(elem) == false && _buffer.size() >= _bufferSize) {
			sprite = elem;
			std::shared_ptr<TextComponent> textComponent = sprite->getComponent<TextComponent>();
			textComponent->_text = text;
			textComponent->_textBack = text;
			textComponent->_type = edit ? TextType::EDIT : TextType::LABEL;
			textComponent->_scale = scale;
			std::shared_ptr<ObjectComponent> objectComponent = sprite->getComponent<ObjectComponent>();
			objectComponent->_sceneX = x;
			objectComponent->_sceneY = y;
			objectComponent->_objectWidth = width;
			objectComponent->_objectHeight = height;
			break;
		}
	}

	if (!sprite) {
		sprite = std::make_shared<Entity>();
		Shader shader;
		GLuint program;
		program = shader.buildProgramFromAsset("../data/shaders/text.vsh", "../data/shaders/text.fsh");
		std::shared_ptr<ObjectComponent> objectComponent(new ObjectComponent());
		objectComponent->initialize(x, y, width, height, program);
		objectComponent->_cameraCoefSpeed = 0;
		sprite->addComponent(objectComponent);
		std::shared_ptr<TextComponent> textComponent(new TextComponent());
		std::shared_ptr<TextLoader> textLoader = std::make_shared<TextLoader>();
		textLoader->bufferSymbols(48);
		textComponent->initialize(textLoader, text, scale, { 1, 0, 0 }, edit ? TextType::EDIT : TextType::LABEL);
		sprite->addComponent(textComponent);
		sprite->createComponent<ClickInsideComponent>()->initialize(false);
		sprite->createComponent<GroupEntitiesComponent>()->initialize(0, "Default");
		_buffer.push_back(sprite);
	}

	return sprite;
}

//should create clickable component labels + bind events from ComponentFunctors for each component for ClickInside
void ComponentTextEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	//Need take all attached components and print them
	int index = 0;
	int height = 50;
	int width = 200;

	for (auto &component : targetEntity->getComponents()) {
		if (component->_componentName != "Component") {
			auto entity = TextHelper::instance()->createText(component->_componentName, 1500, 300 + height * index++, width, height, 0.4, false);
			std::shared_ptr<ClickInsideComponent> clickInsideComponent = entity->getComponent<ClickInsideComponent>();
			clickInsideComponent->_event = std::make_pair(componentFunctors[component->_componentName], targetEntity);
			TextHelper::instance()->attachText(entity);
		}
	}
}

void TextureListEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int height = 50;
	int width = 400;

	if (TextureManager::instance()->getTextureList().size() == 0) {
		auto entity = TextHelper::instance()->createText("Texture list is empty", 1500, 300 + height * index++, width, height, 0.4, false);
		TextHelper::instance()->attachText(entity);
		return;
	}

	for (auto &texture : TextureManager::instance()->getTextureList()) {
		auto entity = TextHelper::instance()->createText(texture->getPath(), 1500, 300 + height * index++, width, height, 0.4, false);
		TextHelper::instance()->attachText(entity);
		entity = TextHelper::instance()->createText("ID: " + std::to_string(texture->getTextureID()) + " Size " + std::to_string(texture->getWidth()) + "x" + std::to_string(texture->getHeight()), 
			1500, 300 + height * index++, width, height, 0.2, false);
		TextHelper::instance()->attachText(entity);
		entity = TextHelper::instance()->createText("Atlas ID: " + std::to_string(texture->getAtlas()->getAtlasID()) + " Atlas pos " + std::to_string(texture->getPosXAtlas()) + "x" + std::to_string(texture->getPosYAtlas()),
			1500, 300 + height * index++, width, height, 0.2, false);
		TextHelper::instance()->attachText(entity);
	}
	
}


void TextureLoadEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int height = 50;
	int width = 400;
	int x = 1500;
	int y = 300;

	std::shared_ptr<TextureAcceptLoadEvent> accept = std::make_shared<TextureAcceptLoadEvent>();

	std::shared_ptr<TextCallback> path = std::make_shared<TextCallback>();
	path->setValue(&accept->_fullPath);
	TextHelper::instance()->getValue(path, "File name", x, y, width, height, 0.4f);

	std::shared_ptr<TextCallback> atlas = std::make_shared<TextCallback>();
	atlas->setValue(&accept->_atlasID);
	TextHelper::instance()->getValue(atlas, "Atlas ID", x, y + height * 1, width, height, 0.4f);

	std::shared_ptr<TextCallback> callbackX = std::make_shared<TextCallback>();
	callbackX->setValue(&accept->_objectX);
	TextHelper::instance()->getValue(callbackX, "x in atlas", x, y + height * 2, width, height, 0.4f);

	std::shared_ptr<TextCallback> callbackY = std::make_shared<TextCallback>();
	callbackY->setValue(&accept->_objectY);
	TextHelper::instance()->getValue(callbackY, "y in atlas", x, y + height * 3, width, height, 0.4f);

	std::shared_ptr<TextCallback> callbackType = std::make_shared<TextCallback>();
	callbackType->setValue(&accept->_type);
	TextHelper::instance()->getValue(callbackType, "Texture/Animated texture (0, 1)", x, y + height * 4, width, height, 0.4f);

	std::shared_ptr<TextCallback> callbackTileX = std::make_shared<TextCallback>();
	callbackTileX->setValue(&accept->_tileX);
	TextHelper::instance()->getValue(callbackTileX, "Tile x number", x, y + height * 5, width, height, 0.4f);

	std::shared_ptr<TextCallback> callbackTileY = std::make_shared<TextCallback>();
	callbackTileY->setValue(&accept->_tileY);
	TextHelper::instance()->getValue(callbackTileY, "Tile y number", x, y + height * 6, width, height, 0.4f);

	auto entity = TextHelper::instance()->createText("Submit", x, y + height * 7, width, height, 0.4, false);
	std::shared_ptr<ClickInsideComponent> clickInsideComponent = entity->getComponent<ClickInsideComponent>();
	clickInsideComponent->_event = std::make_pair(accept, targetEntity);
	TextHelper::instance()->attachText(entity);
}

void TextureAcceptLoadEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	if (_type == 0)
		TextureManager::instance()->loadTexture(std::string("../data/textures/") + _fullPath, _atlasID, _objectX, _objectY);
	else if (_type == 1)
		TextureManager::instance()->loadTexture(std::string("../data/textures/") + _fullPath, _atlasID, _objectX, _objectY, _tileX, _tileY);
}

void SaveEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int height = 50;
	int width = 400;
	int x = 1500;
	int y = 300;

	auto saveLoadComponent = targetEntity->getComponent<SaveLoadComponent>();
	saveLoadComponent->_mode = 0;
	std::shared_ptr<TextCallback> path = std::make_shared<TextCallback>();
	path->setValue(&saveLoadComponent->_path);
	TextHelper::instance()->getValue(path, "File name", x, y, width, height, 0.4f);
}

void LoadEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int height = 50;
	int width = 400;
	int x = 1500;
	int y = 300;

	auto saveLoadComponent = targetEntity->getComponent<SaveLoadComponent>();
	saveLoadComponent->_mode = 1;
	std::shared_ptr<TextCallback> path = std::make_shared<TextCallback>();
	path->setValue(&saveLoadComponent->_path);
	TextHelper::instance()->getValue(path, "File name", x, y, width, height, 0.4f);
}

void AddComponentEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	std::shared_ptr<Component> addedComponent = componentFunctors[_name]->createFunctor(targetEntity);
	targetEntity->addComponent(addedComponent);
}