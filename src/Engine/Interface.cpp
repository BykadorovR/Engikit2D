#include "Interface.h"
#include "Decorator.h"
#include "List.h"

bool ViewDecorators::initialize(std::shared_ptr<Scene> scene) {
	_factories.push_back(std::make_shared<ScrollerVerticalDecoratorFactory>(scene));
	_factories.push_back(std::make_shared<HeaderDecoratorFactory>(scene));
	_factories.push_back(std::make_shared<LabelFactory>(scene));
	_factories.push_back(std::make_shared<BackFactory>(scene));
	_factories.push_back(std::make_shared<ListFactory>(scene, getFactory<LabelFactory>()));
	return false;
}

MainInterface::MainInterface() {
	
}

bool MainInterface::initialize() {
	return false;
}