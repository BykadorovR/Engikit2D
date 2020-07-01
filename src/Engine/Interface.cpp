#include "Interface.h"

bool ViewDecorators::initialize(std::shared_ptr<Scene> scene) {
	_factories.push_back(std::make_shared<ScrollerVerticalDecoratorFactory>(scene));
	_factories.push_back(std::make_shared<HeaderDecoratorFactory>(scene));
	_factories.push_back(std::make_shared<LabelFactory>(scene));
	_factories.push_back(std::make_shared<BackFactory>(scene));
	_factories.push_back(std::make_shared<ListFactory>(scene, getFactory<LabelFactory>()));
	return false;
}

bool ComplexList::initialize(std::tuple<float, float> position, std::tuple<int, int> itemSize, std::shared_ptr<ViewDecorators> viewDecorators) {
	int listItems = 4;
	for (int i = 0; i < listItems; i++) {
		std::shared_ptr<Back> item = std::dynamic_pointer_cast<Back>(viewDecorators->getFactory<BackFactory>()->createView());
		item->initialize();
		item->setSize({ std::get<0>(itemSize), std::get<1>(itemSize) / 4 });
		item->setColorMask({ 0, 0, 0, 0 });
		item->setColorAddition({ 1, 0, (float)(i % 2), 1 });
		_back.push_back(item);
	}

	_list = std::dynamic_pointer_cast<List>(viewDecorators->getFactory<ListFactory>()->createView());
	_list->initialize();
	_list->setSize(itemSize);
	//20 - size of decorator
	_list->setPosition(position);
	_list->setEditable(true);
	_list->addItem("Test");
	_list->addItem("Test2");
	_list->addItem("Test3");
	_list->addItem("Test4");
	_list->addItem("Test5");
	_verticalScrollerDecorator = std::dynamic_pointer_cast<ScrollerVerticalDecorator>(
		viewDecorators->getFactory<ScrollerVerticalDecoratorFactory>()->createView(
			"ScrollerVerticalDecorator", _list));
	_verticalScrollerDecorator->initialize();

	_headerDecorator = std::dynamic_pointer_cast<HeaderDecorator>(
		viewDecorators->getFactory<HeaderDecoratorFactory>()->createView(
			"HeaderDecorator", _list));
	_headerDecorator->initialize();
	_headerDecorator->setText({ "Component's list" });

	for (int i = 0; i < _back.size(); i++) {
		_back[i]->setPosition({ std::get<0>(_list->getPosition()), std::get<1>(_list->getPosition()) + i * std::get<1>(itemSize) / _back.size() });
	}

	return false;
}

bool MainInterface::initialize(std::shared_ptr<Scene> scene) {
	_viewDecorators = std::make_shared<ViewDecorators>();
	_viewDecorators->initialize(scene);

	std::tuple<int, int> listItemSize = { 150, 100 };
	//Create components list
	std::shared_ptr<ComplexList> componentsList = std::make_shared<ComplexList>();
	componentsList->initialize({ std::get<0>(currentResolution) - std::get<0>(listItemSize) - 20, std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) }, 
							   listItemSize, _viewDecorators);

	return false;
}