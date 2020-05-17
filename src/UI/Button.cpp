#include "Button.h"
#include "UserInputComponents.h"
#include "InteractionComponents.h"

Button::Button(std::string name) {
	_viewName = name;
}

/*
List of operation-actions:
1) if "click inside" then "action, set via addClickAction() method"
*/
bool Button::initialize() {
	getBack()->initialize();
	getBack()->setColorMask({0, 0, 0, 0});
	getBack()->setColorAddition({1, 0, 1, 1});
	getLabel()->initialize();
	getLabel()->setText("Text");
	getLabel()->setColor({1, 1, 1, 1});
	getLabel()->setScale(1);

	//--- 1
	getBack()->getEntity()->createComponent<MouseComponent>();
	_clickInside = std::make_shared<ExpressionOperation>();
	_clickInside->addArgument(getBack()->getEntity(), "", "");
	_clickInside->initializeOperation("CLICK ${0}");
	getBack()->getEntity()->createComponent<InteractionComponent>()->attachOperation(_clickInside, InteractionType::MOUSE_START);
	//--- 1
	return false;
}

bool Button::setTexture(int textureID) {
	getBack()->setTexture(textureID);
	return false;
}

bool Button::addClickAction(std::shared_ptr<Action> action) {
	_clickInside->registerAction(action);
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

ButtonFactory::ButtonFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
	_backFactory = std::make_shared<BackFactory>(activeScene);
	_labelFactory = std::make_shared<LabelFactory>(activeScene);
}

std::shared_ptr<View> ButtonFactory::createView(std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<Button> button = std::make_shared<Button>();
	button->setBack(_backFactory->createView());
	button->setLabel(_labelFactory->createView());
	button->setParent(parent);
	return button;
}