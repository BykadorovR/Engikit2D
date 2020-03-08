#include "Label.h"
#include "Shader.h"

Label::Label(std::string name = "Label") {
	_viewName = name;
}

bool Label::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::string text, std::shared_ptr<GlyphsLoader> glyphs) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize(position, size, bufferManager, ShaderStore::instance()->getShader("texture"));
	_entity->getComponent<TextComponent>()->initialize(text, glyphs, bufferManager);
	//TODO: how to pass texture here?
	return false;
}

std::tuple<float, float> Label::getPosition() {
	return _entity->getComponent<ObjectComponent>()->getPosition();
}

std::tuple<float, float> Label::getSize() {
	return _entity->getComponent<ObjectComponent>()->getSize();
}

bool Label::setColor(std::vector<float> color) {
	_entity->getComponent<TextComponent>()->setColor(color);
	return false;
}

bool Label::setScale(float scale) {
	_entity->getComponent<TextComponent>()->setScale(scale);
	return false;
}

bool Label::setPageNumber(int pageNumber) {
	_entity->getComponent<TextComponent>()->setPageNumber(pageNumber);
	return false;
}

bool Label::setTextAllignment(std::tuple<TextAllignment, TextAllignment> allignment) {
	_entity->getComponent<TextComponent>()->setAllignment(allignment);
	return false;
}

bool Label::setLineSpacingCoeff(float coeff) {
	_entity->getComponent<TextComponent>()->setLineSpacingCoeff(coeff);
	return false;
}

LabelFactory::LabelFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
	_backFactory = std::make_shared<BackFactory>(activeScene);
}

std::shared_ptr<View> LabelFactory::createView(std::string name) {
	std::shared_ptr<Label> label = std::make_shared<Label>(name);
	label->setEntity(_activeScene->createEntity());
	label->getEntity()->createComponent<ObjectComponent>();
	label->getEntity()->createComponent<TextComponent>();
	return label;
}

