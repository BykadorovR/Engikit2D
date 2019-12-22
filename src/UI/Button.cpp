#include "Button.h"

ButtonFactory::ButtonFactory(std::shared_ptr<Scene> activeScene) {
	_activeScene = activeScene;
}

std::shared_ptr<View> ButtonFactory::createView() {
	std::shared_ptr<Button> button = std::make_shared<Button>();
	button->setEntity(_activeScene->createEntity());
	button->getEntity()->createComponent<ObjectComponent>();
	button->getEntity()->createComponent<TextureComponent>();
	return button;
}