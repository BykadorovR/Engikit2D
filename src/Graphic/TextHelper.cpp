#include "GraphicComponent.h"
#include "UIComponent.h"
#include "TextHelper.h"
#include "ComponentFunctors.h"

void TextCallback::setValue(float* valueF, TextConversion conversion) {
	_valueF = valueF;
	_conversion = conversion;
}

void TextCallback::setValue(int* valueI, TextConversion conversion) {
	_valueI = valueI;
	_conversion = conversion;
}

void TextCallback::setValue(std::string* valueS, TextConversion conversion) {
	_valueS = valueS;
	_conversion = conversion;
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

void TextHelper::getValue(std::shared_ptr<TextCallback> callback) {
	shared_ptr<Entity> sprite = nullptr;
	for (auto elem : _buffer) {
		if (elem.use_count() == 1) {
			sprite = elem;
			break;
		}
	}
	if (!sprite)
		sprite = createText("Enter", true);

	std::shared_ptr<TextComponent> textComponent = sprite->getComponent<TextComponent>();
	textComponent->setCallback(callback);
}

TextHelper* TextHelper::instance() {
	static TextHelper helper;
	return &helper;
}

TextHelper::TextHelper() {
	//need to create some amount of text objects to use
	for (int i = 0; i < _bufferSize; i++) {
		auto sprite = createText("");
		attachText(sprite);
		_buffer.push_back(sprite);
	}
}

void TextHelper::attachText(std::shared_ptr<Entity> entity) {
	world.registerEntity(entity);
}

void TextHelper::detachText(std::shared_ptr<Entity> entity) {
	world.unregisterEntity(entity);
}

shared_ptr<Entity> TextHelper::createText(std::string text, bool edit) {
	shared_ptr<Entity> sprite = nullptr;
	for (auto &elem : _buffer) {
		int count = elem.use_count();
		//buffer + world
		if (count <= 2 && _buffer.size() == _bufferSize) {
			sprite = elem;
			break;
		}
	}


	if (!sprite) {
		sprite = std::make_shared<Entity>();
		Shader shader;
		GLuint program;
		program = shader.buildProgramFromAsset("../data/shaders/text.vsh", "../data/shaders/text.fsh");
		std::shared_ptr<ObjectComponent> objectComponent(new ObjectComponent());
		objectComponent->initialize(rand() % 1000, rand() % 1000 + 300, 100, 100, program);
		objectComponent->_cameraCoefSpeed = 0;
		sprite->addComponent(objectComponent);
		std::shared_ptr<TextComponent> textComponent(new TextComponent());
		std::shared_ptr<TextLoader> textLoader = std::make_shared<TextLoader>();
		textLoader->bufferSymbols(48);
		textComponent->initialize(textLoader, text, 0.5, { 1, 0, 0 }, edit ? TextType::EDIT : TextType::LABEL);
		sprite->addComponent(textComponent);
		sprite->createComponent<ClickInsideComponent>()->initialize(false);
		sprite->createComponent<GroupEntitiesComponent>()->initialize(0, "Default");
	}

	std::shared_ptr<TextComponent> textComponent = sprite->getComponent<TextComponent>();
	textComponent->_text = text;
	textComponent->_type = edit ? TextType::EDIT : TextType::LABEL;

	return sprite;
}

//should create clickable component labels + bind events from ComponentFunctors for each component for ClickInside
void ComponentTextEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	auto objectEntity = TextHelper::instance()->createText("ObjectComponent", false);
	std::shared_ptr<ClickInsideComponent> clickInsideComponent = objectEntity->getComponent<ClickInsideComponent>();
	clickInsideComponent->_event = std::make_pair(componentFunctors["ObjectComponent"], targetEntity);
}