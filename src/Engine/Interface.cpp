#include "Interface.h"
#include "Decorator.h"
#include "List.h"
#include "State.h"

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

bool MainInterface::initialize(std::shared_ptr<Scene> scene) {
	_viewDecorators = std::make_shared<ViewDecorators>();
	_viewDecorators->initialize(scene);
	std::tuple<int, int> listItemSize = { 150, 100 };
	//Create components list
	for (int i = 0; i < 4; i++) {
		std::shared_ptr<Back> componentsBack = std::dynamic_pointer_cast<Back>(_viewDecorators->getFactory<BackFactory>()->createView());
		componentsBack->initialize();
		componentsBack->setPosition({ std::get<0>(currentResolution) - std::get<0>(listItemSize) - 20, std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) + i * std::get<1>(listItemSize) / 4 });
		componentsBack->setSize({ std::get<0>(listItemSize), std::get<1>(listItemSize) / 4 });
		componentsBack->setColorMask({ 0, 0, 0, 0 });
		componentsBack->setColorAddition({ 1, 0, (float)(i % 2), 1 });
	}
	std::shared_ptr<List> componentsList = std::dynamic_pointer_cast<List>(_viewDecorators->getFactory<ListFactory>()->createView());
	componentsList->initialize();
	componentsList->setSize(listItemSize);
	//20 - size of decorator
	componentsList->setPosition({ std::get<0>(currentResolution) - std::get<0>(listItemSize) - 20, std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) });
	componentsList->setEditable(true);
	componentsList->addItem("Test");
	componentsList->addItem("Test2");
	componentsList->addItem("Test3");
	componentsList->addItem("Test4");
	componentsList->addItem("Test5");
	std::shared_ptr<ScrollerVerticalDecorator> componentsListVerticalScroller = std::dynamic_pointer_cast<ScrollerVerticalDecorator>(_viewDecorators->getFactory<ScrollerVerticalDecoratorFactory>()->createView("ScrollerVerticalDecorator", componentsList));
	componentsListVerticalScroller->initialize();
	std::shared_ptr<HeaderDecorator> componentsListHeader = std::dynamic_pointer_cast<HeaderDecorator>(_viewDecorators->getFactory<HeaderDecoratorFactory>()->createView("HeaderDecorator", componentsList));
	componentsListHeader->initialize();
	componentsListHeader->setText({ "Component's list" });

	return false;
}