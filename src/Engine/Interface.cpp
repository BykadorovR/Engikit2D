#include "Interface.h"
#include <numeric>

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
		viewDecorators->getFactory<HeaderDecoratorFactory>()->createGrid(
			dim, "HeaderDecorator", _list));
	_headerDecorator->initialize();
	_headerDecorator->setText({ "List" });

	return false;
}

bool ComplexList::setPosition(std::tuple<float, float> position) {
	_list->setPosition(position);
	for (int i = 0; i < _back.size(); i++) {
		_back[i]->setPosition({ std::get<0>(_list->getPosition()), std::get<1>(_list->getPosition()) + i * std::get<1>(_back[i]->getSize()) });
	}
	_headerDecorator->getBack()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(_headerDecorator->getBack()->getSize()) });
	_headerDecorator->getLabel()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(_headerDecorator->getLabel()->getSize()) });

	auto backSize = std::get<0>(_back[0]->getSize());
	float listSizeX = std::accumulate(backSize.begin(), backSize.end(), 0.0f);
	_verticalScrollerDecorator->getScrollerUp()->setPosition({ std::get<0>(position) + listSizeX, std::get<1>(position) });
	_verticalScrollerDecorator->getScrollerProgress()->setPosition({ std::get<0>(position) + listSizeX,
																	 std::get<1>(_verticalScrollerDecorator->getScrollerUp()->getPosition()) + std::get<1>(_verticalScrollerDecorator->getScrollerUp()->getSize()) });
	_verticalScrollerDecorator->getScrollerDown()->setPosition({ std::get<0>(position) + listSizeX, 
																 std::get<1>(position) + std::get<1>(_back[0]->getSize()) * _back.size() - std::get<1>(_verticalScrollerDecorator->getScrollerDown()->getSize()) });
	return false;
}

bool ComplexList::setSize(std::tuple<std::vector<float>, float> size) {
	_list->setSize(size);
	for (int i = 0; i < _back.size(); i++)
		_back[i]->setSize({ std::get<0>(size), std::get<1>(size) / _list->getViews().size() });

	std::tuple<float, float> headerSize = { std::accumulate(std::get<0>(size).begin(), std::get<0>(size).end(), 0.0f), std::get<1>(size) / _list->getViews().size() };
	_headerDecorator->getBack()->setSize(headerSize);
	_headerDecorator->getLabel()->setSize(headerSize);

	return false;
}

bool ComplexList::setHeader(std::vector<std::string> text) {
	_headerDecorator->setText(text);
	return false;
}

std::tuple<std::vector<float>, float> ComplexList::getSize() {
	auto size = _back[0]->getSize();
	return { std::get<0>(size), std::get<1>(size) * _back.size() };
}

std::tuple<float, float> ComplexList::getComplexSize() {
	auto size = _back[0]->getSize();
	auto sumSize = std::accumulate(std::get<0>(size).begin(), std::get<0>(size).end(), 0.0f);

	auto scrollerSize = _verticalScrollerDecorator->getScrollerUp()->getSize();
	auto headerSize = _headerDecorator->getBack()->getSize();
	
	return { sumSize + std::get<0>(scrollerSize), (std::get<1>(size) * _back.size()) + std::get<1>(headerSize) };
}

std::tuple<float, float> ComplexList::getPosition() {
	return _list->getPosition();
}

std::tuple<float, float> ComplexList::getComplexPosition() {
	return _headerDecorator->getBack()->getPosition();
}

std::vector<std::shared_ptr<Grid> > ComplexList::getBack() {
	return _back;
}

std::shared_ptr<List> ComplexList::getList() {
	return _list;
}

std::shared_ptr<HeaderDecorator> ComplexList::getHeaderDecorator() {
	return _headerDecorator;
}

std::shared_ptr<ScrollerVerticalDecorator> ComplexList::getVerticalScrollerDecorator() {
	return _verticalScrollerDecorator;
}

bool ComplexLabel::initialize(std::shared_ptr<ViewDecorators> viewDecorators) {
	_name = "Empty";

	_back = std::dynamic_pointer_cast<Back>(viewDecorators->getFactory<BackFactory>()->createView());
	_back->initialize();
	_back->setColorMask({ 0, 0, 0, 0 });
	_back->setColorAddition({ 1, 0, 0, 1 });

	_label = std::dynamic_pointer_cast<Label>(viewDecorators->getFactory<LabelFactory>()->createView());
	_label->initialize();
	_label->setText("Label");
	_label->setEditable(true);

	_headerDecorator = std::dynamic_pointer_cast<HeaderDecorator>(
		viewDecorators->getFactory<HeaderDecoratorFactory>()->createView(
			"HeaderDecorator", _label));
	_headerDecorator->initialize();
	_headerDecorator->setText({ "Header" });

	return false;
}

std::tuple<float, float> ComplexLabel::getSize() {
	return _back->getSize();
}

std::tuple<float, float> ComplexLabel::getComplexSize() {
	auto size = _back->getSize();
	auto headerSize = _headerDecorator->getBack()->getSize();

	return { std::get<0>(size), std::get<1>(size) + std::get<1>(headerSize) };
}

std::tuple<float, float> ComplexLabel::getPosition() {
	return _back->getPosition();
}

std::tuple<float, float> ComplexLabel::getComplexPosition() {
	return _headerDecorator->getBack()->getPosition();
}

std::shared_ptr<Back> ComplexLabel::getBack() {
	return _back;
}

std::shared_ptr<Label> ComplexLabel::getLabel() {
	return _label;
}

std::shared_ptr<HeaderDecorator> ComplexLabel::getHeaderDecorator() {
	return _headerDecorator;
}

bool ComplexLabel::setPosition(std::tuple<float, float> position) {
	_label->setPosition(position);
	_back->setPosition(position);

	_headerDecorator->getBack()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(_headerDecorator->getBack()->getSize()) });
	_headerDecorator->getLabel()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(_headerDecorator->getLabel()->getSize()) });
	return false;
}

bool ComplexLabel::setSize(std::tuple<float, float> size) {
	_label->setSize(size);
	_back->setSize(size);

	_headerDecorator->getBack()->setSize(std::tuple<float, float>(std::get<0>(size), std::get<1>(size)));
	_headerDecorator->getLabel()->setSize(std::tuple<float, float>(std::get<0>(size), std::get<1>(size)));
	return false;
}

bool ComplexLabel::setHeader(std::vector<std::string> text) {
	_headerDecorator->setText(text);
	return false;
}

std::string ComplexLabel::getName() {
	return _name;
}

bool ComplexLabel::setName(std::string name) {
	_name = name;
	return false;
}

bool ComplexLabel::setText(std::string text) {
	_label->setText(text);
	return false;
}

bool MainInterface::initialize(std::shared_ptr<Scene> scene) {
	_viewDecorators = std::make_shared<ViewDecorators>();
	_viewDecorators->initialize(scene);
	std::tuple<std::vector<float>, float> listSize = { {300}, 100 };
	_componentsList = std::make_shared<ComplexList>();
	_componentsList->initialize({ 1, 1 }, _viewDecorators);
	_componentsList->setSize(listSize);
	_componentsList->setPosition({ std::get<0>(currentResolution) - std::get<0>(listSize)[0] - std::get<0>(_componentsList->getVerticalScrollerDecorator()->getScrollerUp()->getSize()),
								   std::get<1>(_componentsList->getHeaderDecorator()->getBack()->getSize()) });
	_componentsList->setHeader({ "Components list" });

	_fieldsList = std::make_shared<ComplexList>();
	_fieldsList->initialize({ 1, 1 }, _viewDecorators);
	_fieldsList->setSize(listSize);
	_fieldsList->setPosition({ std::get<0>(_componentsList->getPosition()),
							   std::get<1>(_componentsList->getPosition()) + std::get<1>(_componentsList->getSize()) + std::get<1>(_componentsList->getHeaderDecorator()->getBack()->getSize()) });
	_fieldsList->setHeader({ "Fields list" });

	_operationsList = std::make_shared<ComplexList>();
	_operationsList->initialize({ 1, 1 }, _viewDecorators);
	_operationsList->setSize(listSize);
	_operationsList->setPosition({ std::get<0>(_fieldsList->getPosition()),
								   std::get<1>(_fieldsList->getPosition()) + std::get<1>(_fieldsList->getSize()) + std::get<1>(_componentsList->getHeaderDecorator()->getBack()->getSize()) });
	_operationsList->setHeader({ "Operations list" });

	_commandsList = std::make_shared<ComplexList>();
	_commandsList->initialize({ 1, 1 }, _viewDecorators);
	_commandsList->setSize(listSize);
	_commandsList->setPosition({ std::get<0>(_operationsList->getPosition()),
								std::get<1>(_operationsList->getPosition()) + std::get<1>(_operationsList->getSize()) + std::get<1>(_commandsList->getHeaderDecorator()->getBack()->getSize()) });
	_commandsList->setHeader({ "Commands list" });

	_argumentsList = std::make_shared<ComplexList>();
	_argumentsList->initialize({ 2, 1 }, _viewDecorators);
	_argumentsList->setSize({ {30, 270}, std::get<1>(listSize) });
	_argumentsList->setPosition({ std::get<0>(_commandsList->getPosition()),
								  std::get<1>(_commandsList->getPosition()) + std::get<1>(_commandsList->getSize()) + std::get<1>(_argumentsList->getHeaderDecorator()->getBack()->getSize()) });
	_argumentsList->setHeader({ "#",  "Argument" });

	_argumentTypeLabel = std::make_shared<ComplexLabel>();
	_argumentTypeLabel->initialize(_viewDecorators);
	_argumentTypeLabel->setSize({ 300, std::get<1>(listSize) / 4 });
	_argumentTypeLabel->setPosition({ std::get<0>(_argumentsList->getPosition()),
									  std::get<1>(_argumentsList->getPosition()) + std::get<1>(_argumentsList->getSize()) + std::get<1>(_argumentTypeLabel->getHeaderDecorator()->getBack()->getSize()) });
	_argumentTypeLabel->setHeader({ "Argument type" });

	_entitiesList = std::make_shared<ComplexList>();
	_entitiesList->initialize({ 3, 1 }, _viewDecorators);
	_entitiesList->setSize({ {30, 30, 240}, std::get<1>(listSize) });
	_entitiesList->setPosition({ 0, std::get<1>(_entitiesList->getHeaderDecorator()->getBack()->getSize()) });
	_entitiesList->setHeader({ "#", "ID", "Entity name" });

	_resourcesList = std::make_shared<ComplexList>();
	_resourcesList->initialize({ 3, 1 }, _viewDecorators);
	_resourcesList->setSize({ {30, 30, 240}, std::get<1>(listSize) });
	_resourcesList->setPosition({ 0, std::get<1>(_entitiesList->getPosition()) + std::get<1>(_entitiesList->getSize()) + std::get<1>(_entitiesList->getHeaderDecorator()->getBack()->getSize()) });
	_resourcesList->setHeader({ "#", "ID", "Texture path" });

	_scenesList = std::make_shared<ComplexList>();
	_scenesList->initialize({ 2, 1 }, _viewDecorators);
	_scenesList->setSize({ { 30, 270 }, std::get<1>(listSize) });
	_scenesList->setPosition({ 0, std::get<1>(_argumentTypeLabel->getComplexPosition()) });// + (std::get<1>(_argumentTypeLabel->getComplexSize()) - std::get<1>(_scenesList->getComplexSize())) / 2});
	_scenesList->setHeader({ "#", "Scene name" });
	return false;
}
