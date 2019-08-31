#include "GraphicComponent.h"
#include "UIComponent.h"
#include "TextHelper.h"
#include "ComponentFunctors.h"
#include "Events.h"

void TextCallback::setValue(float* valueF) {
	_valueF = valueF;
	_conversion = MY_FLOAT;
}

void TextCallback::setValue(int* valueI) {
	_valueI = valueI;
	_conversion = MY_INT;
}

void TextCallback::setValue(std::string* valueS) {
	_valueS.push_back(valueS);
	_conversion = MY_STRING;
}

void TextCallback::setValue(std::vector<float>* valueT) {
	_valueT = valueT;
	_conversion = MY_TUPLE;
}

void TextCallback::setValue(bool* valueB) {
	_valueB = valueB;
	_conversion = MY_BOOL;
}

void TextCallback::setValue(std::tuple<float, float>* valueC) {
	_valueC = valueC;
	_conversion = MY_CLICK;
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
		for (auto &text : _valueS) {
			*text = value;
		}
		break;
	case MY_TUPLE: {
			_valueT->clear();
			std::istringstream is1(value);
			float n;
			while (is1 >> n) {
				_valueT->push_back(n);
			}
		}
		break;
	case MY_CLICK: {
			std::istringstream is1(value);
			float n;
			is1 >> n;
			std::get<0>(*_valueC) = n;
			is1 >> n;
			std::get<1>(*_valueC) = n;
		}
		break;
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
}

void TextHelper::attachText(std::shared_ptr<Entity> entity) {
	world.registerEntity(entity);
}

bool TextHelper::detachText(std::shared_ptr<Entity> entity) {
	//Need to clear components to default
	std::shared_ptr<TextComponent> textComponent = entity->getComponent<TextComponent>();
	if (textComponent)
		textComponent->_focus = false;

	std::shared_ptr<ClickInsideComponent> clickInsideComponent = entity->getComponent<ClickInsideComponent>();
	clickInsideComponent->_leftClick = { 0, 0 };
	clickInsideComponent->_rightClick = { 0, 0 };
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
		sprite->createComponent<GroupEntitiesComponent>()->initialize(0, "Engine");
		_buffer.push_back(sprite);
	}

	return sprite;
}

//should create clickable component labels + bind events from ComponentFunctors for each component for ClickInside
void ComponentTextEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	//Need take all attached components and print them
	int index = 0;
	int x = TextHelper::instance()->getX();
	int y = TextHelper::instance()->getY();
	int height = TextHelper::instance()->getHeight();
	int width = TextHelper::instance()->getWidth();
	float size = TextHelper::instance()->getSize();

	for (auto &component : targetEntity->getComponents()) {
		//default component name (not initialized)
		if (component->_componentName != "Component") {
			auto entity = TextHelper::instance()->createText(component->_componentName, x, y + height * index++, width, height, size, false);
			std::shared_ptr<ClickInsideComponent> clickInsideComponent = entity->getComponent<ClickInsideComponent>();
			clickInsideComponent->_event = std::make_pair(componentFunctors[component->_componentName], targetEntity);
			TextHelper::instance()->attachText(entity);
		}
	}
}

void TextureListEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int x = TextHelper::instance()->getX();
	int y = TextHelper::instance()->getY();
	int height = TextHelper::instance()->getHeight();
	int width = TextHelper::instance()->getWidth();
	float size = TextHelper::instance()->getSize();

	if (TextureManager::instance()->getTextureList().size() == 0) {
		auto entity = TextHelper::instance()->createText("Texture list is empty", x, y + height * index++, width, height, size, false);
		TextHelper::instance()->attachText(entity);
		return;
	}

	for (auto &texture : TextureManager::instance()->getTextureList()) {
		auto entity = TextHelper::instance()->createText(texture->getPath(), x, y + height * index++, width, height, size, false);
		TextHelper::instance()->attachText(entity);
		entity = TextHelper::instance()->createText("ID: " + std::to_string(texture->getTextureID()) + " Size " + std::to_string(texture->getWidth()) + "x" + std::to_string(texture->getHeight()),
			x, y + height * index++, width, height, size, false);
		TextHelper::instance()->attachText(entity);
		entity = TextHelper::instance()->createText("Atlas ID: " + std::to_string(texture->getAtlas()->getAtlasID()) + " Atlas pos " + std::to_string(texture->getPosXAtlas()) + "x" + std::to_string(texture->getPosYAtlas()),
			x, y + height * index++, width, height, size, false);
		TextHelper::instance()->attachText(entity);
	}
	
}


void TextureLoadEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int x = TextHelper::instance()->getX();
	int y = TextHelper::instance()->getY();
	int height = TextHelper::instance()->getHeight();
	int width = TextHelper::instance()->getWidth();
	float size = TextHelper::instance()->getSize();

	std::shared_ptr<TextureAcceptLoadEvent> accept = std::make_shared<TextureAcceptLoadEvent>();

	std::shared_ptr<TextCallback> path = std::make_shared<TextCallback>();
	path->setValue(&accept->_fullPath);
	TextHelper::instance()->getValue(path, "File name", x, y, width, height, size);

	std::shared_ptr<TextCallback> atlas = std::make_shared<TextCallback>();
	atlas->setValue(&accept->_atlasID);
	TextHelper::instance()->getValue(atlas, "Atlas ID", x, y + height * 1, width, height, size);

	std::shared_ptr<TextCallback> callbackX = std::make_shared<TextCallback>();
	callbackX->setValue(&accept->_objectX);
	TextHelper::instance()->getValue(callbackX, "x in atlas", x, y + height * 2, width, height, size);

	std::shared_ptr<TextCallback> callbackY = std::make_shared<TextCallback>();
	callbackY->setValue(&accept->_objectY);
	TextHelper::instance()->getValue(callbackY, "y in atlas", x, y + height * 3, width, height, size);

	std::shared_ptr<TextCallback> callbackType = std::make_shared<TextCallback>();
	callbackType->setValue(&accept->_type);
	TextHelper::instance()->getValue(callbackType, "Texture/Animated texture (0, 1)", x, y + height * 4, width, height, size);

	std::shared_ptr<TextCallback> callbackTileX = std::make_shared<TextCallback>();
	callbackTileX->setValue(&accept->_tileX);
	TextHelper::instance()->getValue(callbackTileX, "Tile x number", x, y + height * 5, width, height, size);

	std::shared_ptr<TextCallback> callbackTileY = std::make_shared<TextCallback>();
	callbackTileY->setValue(&accept->_tileY);
	TextHelper::instance()->getValue(callbackTileY, "Tile y number", x, y + height * 6, width, height, size);

	auto entity = TextHelper::instance()->createText("Submit", x, y + height * 7, width, height, size, false);
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

void TextureChangeEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	std::shared_ptr<TextureComponent> textureComponent = targetEntity->getComponent<TextureComponent>();
	std::shared_ptr<ObjectComponent> objectComponent = targetEntity->getComponent<ObjectComponent>();
	textureComponent->initialize(_id, objectComponent->_program);
	if (_latency.size() > 0)
		textureComponent->_tilesLatency = _latency;
	if (_order.size() > 0)
		textureComponent->_tilesOrder = _order;

	int widthTile = textureComponent->_texture->getWidth() / textureComponent->_texture->getColumn();
	int heightTile = textureComponent->_texture->getHeight() / textureComponent->_texture->getRow();
	objectComponent->_objectWidth = widthTile;
	objectComponent->_objectHeight = heightTile;
	
}

void SaveEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int x = TextHelper::instance()->getX();
	int y = TextHelper::instance()->getY();
	int height = TextHelper::instance()->getHeight();
	int width = TextHelper::instance()->getWidth();
	float size = TextHelper::instance()->getSize();

	auto saveLoadComponent = targetEntity->getComponent<SaveLoadComponent>();
	saveLoadComponent->_mode = 0;
	std::shared_ptr<TextCallback> path = std::make_shared<TextCallback>();
	path->setValue(&saveLoadComponent->_path);
	TextHelper::instance()->getValue(path, "File name", x, y, width, height, size);
}

void LoadEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int x = TextHelper::instance()->getX();
	int y = TextHelper::instance()->getY();
	int height = TextHelper::instance()->getHeight();
	int width = TextHelper::instance()->getWidth();
	float size = TextHelper::instance()->getSize();

	auto saveLoadComponent = targetEntity->getComponent<SaveLoadComponent>();
	saveLoadComponent->_mode = 1;
	std::shared_ptr<TextCallback> path = std::make_shared<TextCallback>();
	path->setValue(&saveLoadComponent->_path);
	TextHelper::instance()->getValue(path, "File name", x, y, width, height, size);
}

void MoveEventPlayer::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	std::shared_ptr<CameraComponent> cameraComponent = targetEntity->getComponent<CameraComponent>();
	if (cameraComponent)
		targetEntity->removeComponent<CameraComponent>();

	int index = 0;
	int x = TextHelper::instance()->getX();
	int y = TextHelper::instance()->getY();
	int height = TextHelper::instance()->getHeight();
	int width = TextHelper::instance()->getWidth();
	float size = TextHelper::instance()->getSize();

	auto objectComponent = targetEntity->getComponent<ObjectComponent>();
	auto moveComponent = targetEntity->getComponent<MoveComponent>();
	moveComponent->initialize(PlayerControlled, objectComponent->_program);

	std::shared_ptr<TextCallback> path = std::make_shared<TextCallback>();
	path->setValue(&moveComponent->_speed);
	TextHelper::instance()->getValue(path, "Speed", x, y, width, height, size);
}

void MoveEventHardcoded::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int x = TextHelper::instance()->getX();
	int y = TextHelper::instance()->getY();
	int height = TextHelper::instance()->getHeight();
	int width = TextHelper::instance()->getWidth();
	float size = TextHelper::instance()->getSize();

	auto objectComponent = targetEntity->getComponent<ObjectComponent>();
	auto moveComponent = targetEntity->getComponent<MoveComponent>();
	moveComponent->initialize(StaticallyDefined, objectComponent->_program);

	std::shared_ptr<TextCallback> path = std::make_shared<TextCallback>();
	path->setValue(&moveComponent->_speed);
	TextHelper::instance()->getValue(path, "Speed", x, y + height * index++, width, height, size);

	path = std::make_shared<TextCallback>();
	path->setValue(&moveComponent->_leftClick);
	TextHelper::instance()->getValue(path, "Enter end point", x, y + height * index++, width, height, size);
}

void AddComponentEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	std::shared_ptr<Component> addedComponent = componentFunctors[_name]->createFunctor(targetEntity);
	targetEntity->addComponent(addedComponent);
}