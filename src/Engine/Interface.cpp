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

bool ComplexList::initialize(std::tuple<int, int> dim, std::shared_ptr<ViewDecorators> viewDecorators) {
	_name = "Empty";
	_viewDecorators = viewDecorators;

	int listItems = 4;
	for (int i = 0; i < listItems; i++) {
		std::shared_ptr<Grid> item = std::dynamic_pointer_cast<Grid>(viewDecorators->getFactory<BackFactory>()->createGrid(dim));
		item->initialize();
		for (auto back : item->getViews()) {
			std::dynamic_pointer_cast<Back>(back)->setColorMask({ 0, 0, 0, 0 });
			std::dynamic_pointer_cast<Back>(back)->setColorAddition({ 1, 0, (float)(i % 2), 1 });
		}
		_back.push_back(item);
	}

	_list = std::dynamic_pointer_cast<List>(viewDecorators->getFactory<ListFactory>()->createGrid(dim));
	_list->initialize();
	_list->setEditable(true);
	for (int i = 0; i < 6; i++) {
		std::vector<std::string> text(std::get<0>(dim), "Test" + std::to_string(i));
		_list->addItem(text);
	}
	_verticalScrollerDecorator = std::dynamic_pointer_cast<ScrollerVerticalDecorator>(
		viewDecorators->getFactory<ScrollerVerticalDecoratorFactory>()->createView(
			"ScrollerVerticalDecorator", _list));
	_verticalScrollerDecorator->initialize();

	_headerDecorator = std::dynamic_pointer_cast<HeaderDecorator>(
		viewDecorators->getFactory<HeaderDecoratorFactory>()->createView(
			"HeaderDecorator", _list));
	_headerDecorator->initialize();
	_headerDecorator->setText({ "List" });

	return false;
}

bool ComplexList::setPosition(std::tuple<float, float> position) {
	_list->setPosition(position);
	for (int i = 0; i < _back.size(); i++) {
		_back[i]->setPosition({ std::get<0>(_list->getPosition()), std::get<1>(_list->getPosition()) + i * std::get<1>(_back[i]->getSize()) });
	}
	return false;
}

bool ComplexList::setSize(std::tuple<std::vector<float>, float> size) {
	_list->setSize(size);
	for (int i = 0; i < _back.size(); i++)
		_back[i]->setSize({ std::get<0>(size), std::get<1>(size) / _list->getViews().size() });

	return false;
}

bool ComplexList::setHeader(std::string text) {
	_headerDecorator->setText({ text });
	return false;
}

std::tuple<float, float> ComplexList::getSize() {
	auto size = _back[0]->getSize();
	return { std::get<0>(size), std::get<1>(size) * _back.size() };
}

std::tuple<float, float> ComplexList::getPosition() {
	return _list->getPosition();
}

std::vector<std::shared_ptr<Grid> > ComplexList::getBack() {
	return _back;
}

std::shared_ptr<List> ComplexList::getList() {
	return _list;
}

bool MainInterface::initialize(std::shared_ptr<Scene> scene) {
	_viewDecorators = std::make_shared<ViewDecorators>();
	_viewDecorators->initialize(scene);

	std::tuple<std::vector<float>, float> listSize = { {150}, 100 };
	_componentsList = std::make_shared<ComplexList>();
	_componentsList->initialize({ 1, 1 }, _viewDecorators);
	_componentsList->setSize(listSize);
	_componentsList->setPosition({ std::get<0>(currentResolution) - std::get<0>(listSize)[0] - 20, std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) });
	_componentsList->setHeader("Components list");

	_fieldsList = std::make_shared<ComplexList>();
	_fieldsList->initialize({ 1, 1 }, _viewDecorators);
	_fieldsList->setSize(listSize);
	_fieldsList->setPosition({ std::get<0>(_componentsList->getPosition()), 
							   std::get<1>(_componentsList->getPosition()) + std::get<1>(_componentsList->getSize()) + std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) });
	_fieldsList->setHeader("Fields list");
	
	_entitiesList = std::make_shared<ComplexList>();
	_entitiesList->initialize({ 3, 1 }, _viewDecorators);
	_entitiesList->setSize({ {20, 100, 100}, 100 });
	_entitiesList->setPosition({ 0, std::ceil(GlyphsLoader::instance().getGlyphHeight() * 1.5) });
	_entitiesList->setHeader("Entities list");

	return false;
}
