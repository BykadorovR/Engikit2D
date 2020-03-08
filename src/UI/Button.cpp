#include "Button.h"

Button::Button(std::string name) {
	_viewName = name;
}

bool Button::initialize(std::tuple<float, float> position, std::tuple<float, float> size, std::vector<float> backColor, std::string text, std::vector<float> textColor, float textScale, std::shared_ptr<GlyphsLoader> glyphLoader) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	getBack()->initialize(position, size, backColor);
	getLabel()->initialize(position, size, text, glyphLoader);
	getLabel()->setColor(textColor);
	getLabel()->setScale(textScale);
	return false;
}

bool Button::initialize(std::tuple<float, float> position, std::tuple<float, float> size, int backTextureID, std::string text, std::vector<float> textColor, float textScale, std::shared_ptr<GlyphsLoader> glyphLoader) {
	std::shared_ptr<BufferManager> bufferManager = std::make_shared<BufferManager>();
	getBack()->initialize(position, size, backTextureID);
	getLabel()->initialize(position, size, text, glyphLoader);
	getLabel()->setColor(textColor);
	getLabel()->setScale(textScale);
	return false;
}

bool Button::setLabel(std::shared_ptr<View> label) {
	_views.push_back(label);
	return false;
}

bool Button::setBack(std::shared_ptr<View> back) {
	_views.push_back(back);
	return false;
}

std::shared_ptr<Label> Button::getLabel() {
	for (auto view : _views) {
		if (view->getName() == "Label")
			return std::dynamic_pointer_cast<Label>(view);
	}
	return nullptr;
}

std::shared_ptr<Back> Button::getBack() {
	for (auto view : _views) {
		if (view->getName() == "Back")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
}

std::vector<std::shared_ptr<View> > Button::getViews() {
	return _views;
}

ButtonFactory::ButtonFactory(std::shared_ptr<Scene> activeScene) {
	_backFactory = std::make_shared<BackFactory>(activeScene);
	_labelFactory = std::make_shared<LabelFactory>(activeScene);
}

std::shared_ptr<View> ButtonFactory::createView(std::string name) {
	std::shared_ptr<Button> button = std::make_shared<Button>();
	button->setBack(std::dynamic_pointer_cast<Back>(_backFactory->createView()));
	button->setLabel(std::dynamic_pointer_cast<Label>(_labelFactory->createView()));
	return button;
}