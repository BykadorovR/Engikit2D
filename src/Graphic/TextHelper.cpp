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

void TextureLoadEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	int index = 0;
	int height = 50;
	int width = 400;

	if (TextureManager::instance()->getTextureList().size() == 0) {
		auto entity = TextHelper::instance()->createText("Texture list is empty", 1500, 300 + height * index++, width, height, 0.4, false);
		TextHelper::instance()->attachText(entity);
	}

	/*for (auto &texture : TextureManager::instance()->getTextureList()) {
		auto entity = TextHelper::instance()->createText(component->_componentName, 1500, 300 + height * index++, width, height, 0.4, false);
	}
	*/
}