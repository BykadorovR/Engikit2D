#include "Button.h"
#include "InteractionComponents.h"

bool Button::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::vector<float> backColor, std::wstring text, std::vector<float> textColor, float textScale, std::shared_ptr<GlyphsLoader> glyphLoader, std::shared_ptr<Shader> shader) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_back->initialize(position, size, backColor, shader);
	_label->initialize(position, size, text, glyphLoader, shader);
	_label->setColor(textColor);
	_label->setScale(textScale);
	return false;
}

bool Button::initialize(std::tuple<float, float> position, std::tuple<float, float> size, int backTextureID, std::wstring text, std::vector<float> textColor, float textScale, std::shared_ptr<GlyphsLoader> glyphLoader, std::shared_ptr<Shader> shader) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	_back->initialize(position, size, backTextureID, shader);
	_label->initialize(position, size, text, glyphLoader, shader);
	_label->setColor(textColor);
	_label->setScale(textScale);
	return false;
}

bool Button::setLabel(std::shared_ptr<Label> label) {
	_label = label;
	return false;
}

bool Button::setBack(std::shared_ptr<Back> back) {
	_back = back;
	return false;
}

std::shared_ptr<Label> Button::getLabel() {
	return _label;
}

std::shared_ptr<Back> Button::getBack() {
	return _back;
}

bool Button::setInteraction(std::shared_ptr<Operation> operation, std::shared_ptr<Action> action) {
	_entity->getComponent<InteractionComponent>()->attachAction(operation, action);
	return false;
}

ButtonFactory::ButtonFactory(std::shared_ptr<Scene> activeScene) {
	_backFactory = std::make_shared<BackFactory>(activeScene);
	_labelFactory = std::make_shared<LabelFactory>(activeScene);
}

std::shared_ptr<View> ButtonFactory::createView() {
	std::shared_ptr<Button> button = std::make_shared<Button>();
	button->setBack(std::dynamic_pointer_cast<Back>(_backFactory->createView()));
	button->setLabel(std::dynamic_pointer_cast<Label>(_labelFactory->createView()));
	return button;
}