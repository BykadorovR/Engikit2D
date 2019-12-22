#pragma once
#include "GraphicComponents.h"
#include "View.h"
#include "Scene.h"

class Button : public View {
public:
};

class ButtonFactory : public ViewFactory {
private:
	std::shared_ptr<Scene> _activeScene;
public:
	ButtonFactory(std::shared_ptr<Scene> activeScene);
	std::shared_ptr<View> createView();
};