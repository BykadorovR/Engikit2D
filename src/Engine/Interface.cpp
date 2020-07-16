#include "Interface.h"
#include <numeric>
#include "UtilityComponents.h"

bool ViewDecorators::initialize(std::shared_ptr<Scene> scene) {
	_factories.push_back(std::make_shared<ScrollerVerticalDecoratorFactory>(scene));
	_factories.push_back(std::make_shared<HeaderDecoratorFactory>(scene));
	_factories.push_back(std::make_shared<CornerButtonDecoratorFactory>(scene));
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

bool ComplexList::initialize(std::tuple<int, int> dim, std::shared_ptr<ViewDecorators> viewDecorators, int decoratorMask) {
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

	if (decoratorMask & VERTICAL) {
		_verticalScrollerDecorator = std::dynamic_pointer_cast<ScrollerVerticalDecorator>(
			viewDecorators->getFactory<ScrollerVerticalDecoratorFactory>()->createView(
				"ScrollerVerticalDecorator", _list));
		_verticalScrollerDecorator->initialize();
	}
	else
		_verticalScrollerDecorator = nullptr;

	if (decoratorMask & HEADER) {
		_headerDecorator = std::dynamic_pointer_cast<HeaderDecorator>(
			viewDecorators->getFactory<HeaderDecoratorFactory>()->createGrid(
				dim, "HeaderDecorator", _list));
		_headerDecorator->initialize();
		_headerDecorator->setText({ "List" });
	}
	else
		_headerDecorator = nullptr;

	if (decoratorMask & CORNER_BUTTON) {
		_cornerButtonDecorator = std::dynamic_pointer_cast<CornerButtonDecorator>(
			viewDecorators->getFactory<CornerButtonDecoratorFactory>()->createView(
				"CornerButtonDecorator", _list));
		_cornerButtonDecorator->initialize();
		_cornerButtonDecorator->setText({ "+" });
	}

	return false;
}

bool ComplexList::setPosition(std::tuple<float, float> position) {
	_list->setPosition(position);
	for (int i = 0; i < _back.size(); i++) {
		_back[i]->setPosition({ std::get<0>(_list->getPosition()), std::get<1>(_list->getPosition()) + i * std::get<1>(_back[i]->getSize()) });
	}
	if (_headerDecorator) {
		_headerDecorator->getBack()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(_headerDecorator->getBack()->getSize()) });
		_headerDecorator->getLabel()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(_headerDecorator->getLabel()->getSize()) });
	}

	if (_verticalScrollerDecorator) {
		auto backSize = std::get<0>(_back[0]->getSize());
		float listSizeX = std::accumulate(backSize.begin(), backSize.end(), 0.0f);
		_verticalScrollerDecorator->getScrollerUp()->setPosition({ std::get<0>(position) + listSizeX, std::get<1>(position) });
		_verticalScrollerDecorator->getScrollerProgress()->setPosition({ std::get<0>(position) + listSizeX,
																		 std::get<1>(_verticalScrollerDecorator->getScrollerUp()->getPosition()) + std::get<1>(_verticalScrollerDecorator->getScrollerUp()->getSize()) });
		_verticalScrollerDecorator->getScrollerDown()->setPosition({ std::get<0>(position) + listSizeX,
																	 std::get<1>(position) + std::get<1>(_back[0]->getSize()) * _back.size() - std::get<1>(_verticalScrollerDecorator->getScrollerDown()->getSize()) });
	}
	return false;
}

bool ComplexList::setSize(std::tuple<std::vector<float>, float> size) {
	_list->setSize(size);
	for (int i = 0; i < _back.size(); i++)
		_back[i]->setSize({ std::get<0>(size), std::get<1>(size) / _list->getViews().size() });

	if (_headerDecorator) {
		std::tuple<float, float> headerSize = { std::accumulate(std::get<0>(size).begin(), std::get<0>(size).end(), 0.0f), std::get<1>(size) / _list->getViews().size() };
		_headerDecorator->getBack()->setSize(headerSize);
		_headerDecorator->getLabel()->setSize(headerSize);
	}
	return false;
}

bool ComplexList::setGroup(std::string name) {
	auto back = getBack();
	for (auto &item : back) {
		for (auto &gridItem : item->getViews()) {
			gridItem->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
		}
	}

	auto listViews = getList()->getViews();
	for (auto &listItem : listViews) {
		if (std::dynamic_pointer_cast<Grid>(listItem)) {
			for (auto &gridItem : listItem->getViews()) {
				gridItem->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
			}
		}
		else {
			listItem->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
		}
	}

	if (_verticalScrollerDecorator) {
		for (auto &item : _verticalScrollerDecorator->getViews()) {
			item->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
		}
	}

	if (_headerDecorator) {
		for (auto &item : _headerDecorator->getViews()) {
			if (std::dynamic_pointer_cast<Grid>(item)) {
				for (auto &gridItem : item->getViews()) {
					gridItem->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
				}
			}
			else {
				item->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
			}
		}
	}

	return false;
}

bool ComplexList::setHeader(std::vector<std::string> text) {
	if (_headerDecorator)
		_headerDecorator->setText(text);
	else
		return true;

	return false;
}

std::tuple<std::vector<float>, float> ComplexList::getSize() {
	auto size = _back[0]->getSize();
	return { std::get<0>(size), std::get<1>(size) * _back.size() };
}

std::tuple<float, float> ComplexList::getComplexSize() {
	auto size = _back[0]->getSize();
	auto sumSize = std::accumulate(std::get<0>(size).begin(), std::get<0>(size).end(), 0.0f);
	
	std::tuple<float, float> scrollerSize = { 0, 0 };
	if (_verticalScrollerDecorator)
		scrollerSize = _verticalScrollerDecorator->getScrollerUp()->getSize();

	std::tuple<std::vector<float>, float> headerSize = { {0}, 0 };
	if (_headerDecorator)
		_headerDecorator->getBack()->getSize();

	return { sumSize + std::get<0>(scrollerSize), (std::get<1>(size) * _back.size()) + std::get<1>(headerSize) };
}

std::tuple<float, float> ComplexList::getPosition() {
	return _list->getPosition();
}

std::tuple<float, float> ComplexList::getComplexPosition() {
	std::tuple<float, float> position;
	if (_headerDecorator)
		position = _headerDecorator->getBack()->getPosition();
	else
		position = _back[0]->getPosition();

	return position;
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

bool ComplexLabel::initialize(std::shared_ptr<ViewDecorators> viewDecorators, int decoratorMask) {
	_name = "Empty";

	_back = std::dynamic_pointer_cast<Back>(viewDecorators->getFactory<BackFactory>()->createView());
	_back->initialize();
	_back->setColorMask({ 0, 0, 0, 0 });
	_back->setColorAddition({ 1, 0, 0, 1 });

	_label = std::dynamic_pointer_cast<Label>(viewDecorators->getFactory<LabelFactory>()->createView());
	_label->initialize();
	_label->setText("Label");
	_label->setEditable(true);

	if (decoratorMask & DecoratorMask::HEADER) {
		_headerDecorator = std::dynamic_pointer_cast<HeaderDecorator>(
			viewDecorators->getFactory<HeaderDecoratorFactory>()->createView(
				"HeaderDecorator", _label));
		_headerDecorator->initialize();
		_headerDecorator->setText({ "Default" });
	}
	else {
		_headerDecorator = nullptr;
	}

	if (decoratorMask & CORNER_BUTTON) {
		_cornerButtonDecorator = std::dynamic_pointer_cast<CornerButtonDecorator>(
			viewDecorators->getFactory<CornerButtonDecoratorFactory>()->createView(
				"CornerButtonDecorator", _label));
		_cornerButtonDecorator->initialize();
		_cornerButtonDecorator->setText({ "+" });
	}
	return false;
}

std::tuple<float, float> ComplexLabel::getSize() {
	return _back->getSize();
}

std::tuple<float, float> ComplexLabel::getComplexSize() {
	auto size = _back->getSize();
	std::tuple<float, float> headerSize = { 0, 0 };
	if (_headerDecorator)
		_headerDecorator->getBack()->getSize();

	return { std::get<0>(size), std::get<1>(size) + std::get<1>(headerSize) };
}

std::tuple<float, float> ComplexLabel::getPosition() {
	return _back->getPosition();
}

std::tuple<float, float> ComplexLabel::getComplexPosition() {
	std::tuple<float, float> position;
	if (_headerDecorator)
		position = _headerDecorator->getBack()->getPosition();
	else
		position = _back->getPosition();

	return position;
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

	if (_headerDecorator) {
		_headerDecorator->getBack()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(_headerDecorator->getBack()->getSize()) });
		_headerDecorator->getLabel()->setPosition({ std::get<0>(position), std::get<1>(position) - std::get<1>(_headerDecorator->getLabel()->getSize()) });
	}
	return false;
}

bool ComplexLabel::setSize(std::tuple<float, float> size) {
	_label->setSize(size);
	_back->setSize(size);

	if (_headerDecorator) {
		_headerDecorator->getBack()->setSize(std::tuple<float, float>(std::get<0>(size), std::get<1>(size)));
		_headerDecorator->getLabel()->setSize(std::tuple<float, float>(std::get<0>(size), std::get<1>(size)));
	}
	return false;
}

bool ComplexLabel::setHeader(std::vector<std::string> text) {
	if (_headerDecorator)
		_headerDecorator->setText(text);
	else
		return true;

	return false;
}

std::string ComplexLabel::getName() {
	return _name;
}

bool ComplexLabel::setName(std::string name) {
	_name = name;
	return false;
}

bool ComplexLabel::setGroup(std::string name) {
	getBack()->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
	getLabel()->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
	if (_headerDecorator) {
		for (auto &item : _headerDecorator->getViews()) {
			if (std::dynamic_pointer_cast<Grid>(item)) {
				for (auto &gridItem : item->getViews()) {
					gridItem->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
				}
			}
			else {
				item->getEntity()->getComponent<GroupComponent>()->setMember("groupName", name);
			}
		}
	}
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
	_componentsList->initialize({ 1, 1 }, _viewDecorators, DecoratorMask::HEADER | DecoratorMask::VERTICAL);
	_componentsList->setGroup("Engine");
	_componentsList->setSize(listSize);
	_componentsList->setPosition({ std::get<0>(currentResolution) - std::get<0>(listSize)[0] - std::get<0>(_componentsList->getVerticalScrollerDecorator()->getScrollerUp()->getSize()),
								   std::get<1>(_componentsList->getHeaderDecorator()->getBack()->getSize()) });
	_componentsList->setHeader({ "Components list" });

	_fieldsList = std::make_shared<ComplexList>();
	_fieldsList->initialize({ 1, 1 }, _viewDecorators, DecoratorMask::HEADER | DecoratorMask::VERTICAL);
	_fieldsList->setGroup("Engine");
	_fieldsList->setSize(listSize);
	_fieldsList->setPosition({ std::get<0>(_componentsList->getPosition()),
							   std::get<1>(_componentsList->getPosition()) + std::get<1>(_componentsList->getSize()) + std::get<1>(_componentsList->getHeaderDecorator()->getBack()->getSize()) });
	_fieldsList->setHeader({ "Fields list" });

	_operationsList = std::make_shared<ComplexList>();
	_operationsList->initialize({ 1, 1 }, _viewDecorators, DecoratorMask::HEADER | DecoratorMask::VERTICAL);
	_operationsList->setGroup("Engine");
	_operationsList->setSize(listSize);
	_operationsList->setPosition({ std::get<0>(_fieldsList->getPosition()),
								   std::get<1>(_fieldsList->getPosition()) + std::get<1>(_fieldsList->getSize()) + std::get<1>(_componentsList->getHeaderDecorator()->getBack()->getSize()) });
	_operationsList->setHeader({ "Operations list" });

	_commandsList = std::make_shared<ComplexList>();
	_commandsList->initialize({ 1, 1 }, _viewDecorators, DecoratorMask::HEADER | DecoratorMask::VERTICAL);
	_commandsList->setGroup("Engine");
	_commandsList->setSize(listSize);
	_commandsList->setPosition({ std::get<0>(_operationsList->getPosition()),
								std::get<1>(_operationsList->getPosition()) + std::get<1>(_operationsList->getSize()) + std::get<1>(_commandsList->getHeaderDecorator()->getBack()->getSize()) });
	_commandsList->setHeader({ "Commands list" });

	_argumentsList = std::make_shared<ComplexList>();
	_argumentsList->initialize({ 2, 1 }, _viewDecorators, DecoratorMask::HEADER | DecoratorMask::VERTICAL);
	_argumentsList->setGroup("Engine");
	_argumentsList->setSize({ {30, 270}, std::get<1>(listSize) });
	_argumentsList->setPosition({ std::get<0>(_commandsList->getPosition()),
								  std::get<1>(_commandsList->getPosition()) + std::get<1>(_commandsList->getSize()) + std::get<1>(_argumentsList->getHeaderDecorator()->getBack()->getSize()) });
	_argumentsList->setHeader({ "#",  "Argument" });

	_argumentTypeLabel = std::make_shared<ComplexLabel>();
	_argumentTypeLabel->initialize(_viewDecorators, DecoratorMask::HEADER | DecoratorMask::CORNER_BUTTON);
	_argumentTypeLabel->setGroup("Engine");
	_argumentTypeLabel->setSize({ 300, std::get<1>(listSize) / 4 });
	_argumentTypeLabel->setPosition({ std::get<0>(_argumentsList->getPosition()),
									  std::get<1>(_argumentsList->getPosition()) + std::get<1>(_argumentsList->getSize()) + std::get<1>(_argumentTypeLabel->getHeaderDecorator()->getBack()->getSize()) });
	_argumentTypeLabel->setHeader({ "Argument type" });

	_entitiesList = std::make_shared<ComplexList>();
	_entitiesList->initialize({ 3, 1 }, _viewDecorators, DecoratorMask::HEADER | DecoratorMask::VERTICAL);
	_entitiesList->setGroup("Engine");
	_entitiesList->setSize({ {30, 30, 240}, std::get<1>(listSize) });
	_entitiesList->setPosition({ 0, std::get<1>(_entitiesList->getHeaderDecorator()->getBack()->getSize()) });
	_entitiesList->setHeader({ "#", "ID", "Entity name" });

	_resourcesList = std::make_shared<ComplexList>();
	_resourcesList->initialize({ 3, 1 }, _viewDecorators, DecoratorMask::HEADER | DecoratorMask::VERTICAL | DecoratorMask::CORNER_BUTTON);
	_resourcesList->setGroup("Engine");
	_resourcesList->setSize({ {30, 30, 240}, std::get<1>(listSize) });
	_resourcesList->setPosition({ 0, std::get<1>(_entitiesList->getPosition()) + std::get<1>(_entitiesList->getSize()) + std::get<1>(_entitiesList->getHeaderDecorator()->getBack()->getSize()) });
	_resourcesList->setHeader({ "#", "ID", "Texture path" });

	_scenesList = std::make_shared<ComplexList>();
	_scenesList->initialize({ 2, 1 }, _viewDecorators, DecoratorMask::HEADER | DecoratorMask::VERTICAL);
	_scenesList->setGroup("Engine");
	_scenesList->setSize({ { 30, 270 }, std::get<1>(listSize) });
	_scenesList->setPosition({ 0, std::get<1>(_argumentTypeLabel->getComplexPosition()) });// + (std::get<1>(_argumentTypeLabel->getComplexSize()) - std::get<1>(_scenesList->getComplexSize())) / 2});
	_scenesList->setHeader({ "#", "Scene name" });

	return false;
}

bool MainInterface::fillEntitiesList(std::shared_ptr<Scene> scene) {
	auto entities = scene->getEntityManager()->getEntities();
	_entitiesList->getList()->clear();
	for (int i = 0; i < entities.size(); i++) {
		std::string groupName = *std::get<0>(std::get<0>(entities[i])->getComponent<GroupComponent>()->getMemberString("groupName"));
		if (groupName != "Engine")
			_entitiesList->getList()->addItem({ std::to_string(i), std::to_string(std::get<0>(entities[0])->getIndex()), std::get<0>(entities[0])->getName() });
	}
	return false;
}