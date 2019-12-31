#include "Label.h"
#include "GraphicComponents.h"

bool Label::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::wstring text, std::shared_ptr<GlyphsLoader> glyphs, std::shared_ptr<Shader> shader) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_entity->getComponent<ObjectComponent>()->initialize(position, size, bufferManager, shader);
	_entity->getComponent<TextComponent>()->initialize(text, glyphs, bufferManager);
	return false;
}

bool Label::setColor(std::vector<float> color) {
	_entity->getComponent<TextComponent>()->setColor(color);
	return false;
}

bool Label::setScale(float scale) {
	_entity->getComponent<TextComponent>()->setScale(scale);
	return false;
}


LabelFactory::LabelFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
}
std::shared_ptr<View> LabelFactory::createView() {
	std::shared_ptr<Label> label = std::make_shared<Label>();
	label->setEntity(_activeScene->createEntity());
	label->getEntity()->createComponent<ObjectComponent>();
	label->getEntity()->createComponent<TextComponent>();
	return label;
}

