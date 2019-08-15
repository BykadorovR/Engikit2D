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

void TextHelper::getValue(std::shared_ptr<TextCallback> callback, World* world) {
	shared_ptr<Entity> sprite = createText("Enter", world, true);
	std::shared_ptr<TextComponent> textComponent = sprite->getComponent<TextComponent>();
	textComponent->setCallback(callback);
}

shared_ptr<Entity> TextHelper::createText(std::string text, World* world, bool edit) {
	shared_ptr<Entity> sprite;
	sprite = world->createEntity();
	Shader shader;
	GLuint program;
	program = shader.buildProgramFromAsset("../data/shaders/text.vsh", "../data/shaders/text.fsh");
	std::shared_ptr<ObjectComponent> objectComponent(new ObjectComponent());
	objectComponent->initialize(500, 400, 100, 100, program);
	objectComponent->_cameraCoefSpeed = 0;
	sprite->addComponent(objectComponent);
	std::shared_ptr<TextComponent> textComponent(new TextComponent());
	std::shared_ptr<TextLoader> textLoader = std::make_shared<TextLoader>();
	textLoader->bufferSymbols(48);
	textComponent->initialize(textLoader, text, 1, { 1, 0, 0 }, edit ? TextType::EDIT : TextType::LABEL);
	sprite->addComponent(textComponent);
	sprite->createComponent<ClickInsideComponent>()->initialize(false);
	sprite->createComponent<GroupEntitiesComponent>()->initialize(0, "Default");
	return sprite;
}

//should create clickable component labels + bind events from ComponentFunctors for each component for ClickInside
void ComponentTextEvent::configureFunctor(std::shared_ptr<Entity> targetEntity) {
	TextHelper helper;
	auto objectEntity = helper.createText("ObjectComponent", &world, false);
	std::shared_ptr<ClickInsideComponent> clickInsideComponent = objectEntity->getComponent<ClickInsideComponent>();
	clickInsideComponent->_event = std::make_pair(componentFunctors["ObjectComponent"], targetEntity);
}