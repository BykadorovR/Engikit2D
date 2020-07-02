#include "Interface.h"

bool ViewDecorators::initialize(std::shared_ptr<Scene> scene) {
	_factories.push_back(std::make_shared<ScrollerVerticalDecoratorFactory>(scene));
	_factories.push_back(std::make_shared<HeaderDecoratorFactory>(scene));
	_factories.push_back(std::make_shared<LabelFactory>(scene));
	_factories.push_back(std::make_shared<BackFactory>(scene));
	_factories.push_back(std::make_shared<ListFactory>(scene, getFactory<LabelFactory>()));
	return false;
}

std::string ComplexList::getName() {
	return _name;
}

bool ComplexList::setName(std::string name) {
	_name = name;
	return false;
}

bool ComplexList::initialize(std::shared_ptr<ViewDecorators> viewDecorators) {
	_name = "Empty";
	_viewDecorators = viewDecorators;

	int listItems = 4;
	for (int i = 0; i < listItems; i++) {
		std::shared_ptr<Back> item = std::dynamic_pointer_cast<Back>(viewDecorators->getFactory<BackFactory>()->createView());
		item->initialize();
		item->setColorMask({ 0, 0, 0, 0 });
		item->setColorAddition({ 1, 0, (float)(i % 2), 1 });
		_back.push_back(item);
	}

	_list = std::dynamic_pointer_cast<List>(viewDecorators->getFactory<ListFactory>()->createView());
	_list->initialize();
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

	return false;
}

bool ComplexList::setPosition(std::tuple<int, int> position) {
	_list->setPosition(position);
	for (int i = 0; i < _back.size(); i++) {
		_back[i]->setPosition({ std::get<0>(_list->getPosition()), std::get<1>(_list->getPosition()) + i * std::get<1>(_back[i]->getSize()) });
	}
	return false;
}

bool ComplexList::setSize(std::tuple<int, int> size) {
	_list->setSize(size);
	for (int i = 0; i < _back.size(); i++)
		_back[i]->setSize({ std::get<0>(size), std::get<1>(size) / 4 });

	return false;
}

std::tuple<int, int> ComplexList::getSize() {
	auto size = _back[0]->getSize();
	return { std::get<0>(size), std::get<1>(size) * _back.size() };
}

std::tuple<int, int> ComplexList::getPosition() {
	return _list->getPosition();
}

std::vector<std::shared_ptr<Back> > ComplexList::getBack() {
	return _back;
}

std::shared_ptr<List> ComplexList::getList() {
	return _list;
}

bool MainInterface::initialize(std::shared_ptr<Scene> scene) {
	_viewDecorators = std::make_shared<ViewDecorators>();
	_viewDecorators->initialize(scene);

	_componentsList = std::make_shared<ComplexList>();
	_componentsList->initialize(_viewDecorators);
	_componentsList->setSize(_listItemSize);
	_componentsList->setPosition({ std::get<0>(currentResolution) - std::get<0>(_listItemSize) - 20, std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) });
	
	_fieldsList = std::make_shared<ComplexList>();
	_fieldsList->initialize(_viewDecorators);
	_fieldsList->setSize(_listItemSize);
	_fieldsList->setPosition({ std::get<0>(_componentsList->getPosition()), 
							   std::get<1>(_componentsList->getPosition()) + std::get<1>(_componentsList->getSize())});

	return false;
}
