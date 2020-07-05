#include "List.h"
#include "GraphicComponents.h"
#include "CustomComponents.h"
#include "InteractionOperations.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"
#include "UserInputComponents.h"
#include "Common.h"

//TODO: add header to list for specifiying entity name/ID, component's name/ID
List::List(std::string name) {
	_viewName = name;
}

std::tuple<float, float> List::getPosition() {
	std::tuple<float, float> position;
	//TODO: remove all getters from view classes ? because we have getters in components
	if (std::dynamic_pointer_cast<Grid>(_views[0])) {
		position = std::dynamic_pointer_cast<Grid>(_views[0])->getPosition();
	}
	else if (std::dynamic_pointer_cast<Label>(_views[0])) {
		position = _views[0]->getEntity()->getComponent<ObjectComponent>()->getPosition();
	}

	return position;
}

bool List::setEditable(bool editable) {
	if (std::dynamic_pointer_cast<Grid>(_views[0])) {
		for (int i = 0; i < _views.size(); i++) {
			std::shared_ptr<Grid> grid = std::dynamic_pointer_cast<Grid>(_views[i]);
			for (auto label : grid->getViews()) {
				std::dynamic_pointer_cast<Label>(label)->setEditable(editable);
			}
		}
	}
	else if (std::dynamic_pointer_cast<Label>(_views[0])) {
		for (int i = 0; i < _views.size(); i++) {
			std::dynamic_pointer_cast<Label>(_views[i])->setEditable(editable);
		}
	}
	return false;
}

bool List::setPosition(std::tuple<float, float> position) {
	if (std::dynamic_pointer_cast<Grid>(_views[0])) {
		std::tuple<float, float> mPosition = position;
		for (int i = 0; i < _views.size(); i++) {
			std::shared_ptr<Grid> grid = std::dynamic_pointer_cast<Grid>(_views[i]);
			grid->setPosition(position);
			std::tuple<float, float> listItemSize = grid->getSize();
			position = { std::get<0>(position), std::get<1>(position) + std::get<1>(listItemSize) };
		}
	}
	else if (std::dynamic_pointer_cast<Label>(_views[0])) {
		//TODO: add getSize/getPosition/setSize/setPosition functions to View class so it's convinied to use it here
		std::tuple<float, float> listItemSize = _views[0]->getEntity()->getComponent<ObjectComponent>()->getSize();
		_views[0]->getEntity()->getComponent<ObjectComponent>()->setMember("positionX", std::get<0>(position));
		_views[0]->getEntity()->getComponent<ObjectComponent>()->setMember("positionY", std::get<1>(position));
		for (int i = 1; i < _views.size(); i++) {
			auto prevPosition = _views[i - 1]->getEntity()->getComponent<ObjectComponent>()->getPosition();
			std::tuple<float, float> currentPosition = { std::get<0>(prevPosition), std::get<1>(prevPosition) + std::get<1>(listItemSize) };

			_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("positionX", std::get<0>(currentPosition));
			_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("positionY", std::get<1>(currentPosition));
		}
	}
	return false;
}

bool List::setSize(std::tuple<std::vector<float>, float> size) {
	if (std::dynamic_pointer_cast<Grid>(_views[0])) {
		for (int j = 0; j < _views.size(); j++) {
			auto childs = std::dynamic_pointer_cast<Grid>(_views[j])->getViews();
			for (int i = 0; i < childs.size(); i++) {
				childs[i]->getEntity()->getComponent<ObjectComponent>()->setMember("sizeX", std::get<0>(size)[i]);
				childs[i]->getEntity()->getComponent<ObjectComponent>()->setMember("sizeY", std::get<1>(size) / _views.size());
			}
		}
	}
	else if (std::dynamic_pointer_cast<Label>(_views[0])) {
		for (int i = 0; i < _views.size(); i++) {
			_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("sizeX", std::get<0>(size)[0]);
			_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("sizeY", std::get<1>(size) / _views.size());
		}
	}
	return false;
}

bool List::addItem(std::vector<std::string> text) {
	if (std::dynamic_pointer_cast<Grid>(_views[0])) {
		//take views from first grid as childs
		auto childs = _views[0]->getViews();
		for (int i = 0; i < childs.size(); i++) {
			childs[i]->getEntity()->getComponent<CustomStringArrayComponent>()->addCustomValue(text[i], "list" + std::to_string(i));
		}
	}
	else if (std::dynamic_pointer_cast<Label>(_views[0])) {
		_views[0]->getEntity()->getComponent<CustomStringArrayComponent>()->addCustomValue(text[0], "list0");
	}
	return false;
}

bool List::clear() {
	_views[0]->getEntity()->getComponent<CustomStringArrayComponent>()->clear("list0");
	for (int i = 0; i < _views.size(); i++)
		_views[i]->getEntity()->getComponent<TextComponent>()->setText("");
	return false;
}

//views and items are different thigs. Views - labels or grids, items - string (text)
bool List::initialize() {
	//first of all need to understand whether views are labels or grids
	if (std::dynamic_pointer_cast<Grid>(_views[0])) {
		auto childs = _views[0]->getViews();
		//first child of first row contain scroller vertical start
		childs[0]->getEntity()->createComponent<CustomFloatComponent>()->addCustomValue("listStartVertical", 0);
		for (int i = 0; i < childs.size(); i++)
			childs[i]->getEntity()->createComponent<CustomStringArrayComponent>()->initializeEmpty("list" + std::to_string(i));

		for (int i = 0; i < _views.size(); i++) {
			_views[i]->initialize();
			auto childs = _views[i]->getViews();
			for (int j = 0; j < childs.size(); j++) {
				//every view is a grid so contain more views
				auto mapText = std::make_shared<ExpressionOperation>();
				//NOTE: we send list without index only because we use SIZE
				mapText->addArgument(_views[0]->getViews()[j]->getEntity(), "CustomStringArrayComponent", "list" + std::to_string(j));
				mapText->addArgument(nullptr, "", std::to_string(i));
				mapText->addArgument(_views[0]->getViews()[0]->getEntity(), "CustomFloatComponent", "listStartVertical");
				mapText->addArgument(childs[j]->getEntity(), "TextComponent", "focus");
				mapText->initializeOperation("${1} + ${2} < SIZE ${0} AND ${3} = 0");
				auto setLine = std::make_shared<AssignAction>();
				setLine->addArgument(childs[j]->getEntity(), "TextComponent", "text");
				//TODO: need to take every column from list and map to appropriate text in view
				setLine->addArgument(_views[0]->getViews()[j]->getEntity(), "CustomStringArrayComponent", "list" + std::to_string(j));
				setLine->addArgument(_views[0]->getViews()[0]->getEntity(), "CustomFloatComponent", "listStartVertical");
				setLine->addArgument(nullptr, "", std::to_string(i));
				setLine->initializeAction("${0} SET ${1} AT ( ${2} + ${3} )");
				mapText->registerAction(setLine);
				//TODO: add common ONCE + mouse/keyboard
				_views[0]->getViews()[j]->getEntity()->createComponent<InteractionComponent>()->attachOperation(mapText, InteractionType::COMMON_END);
			}
		}
	} else if (std::dynamic_pointer_cast<Label>(_views[0])) {
		_views[0]->getEntity()->createComponent<CustomFloatComponent>()->addCustomValue("listStartVertical", 0);
		_views[0]->getEntity()->createComponent<CustomStringArrayComponent>()->initializeEmpty("list0");
		for (int i = 0; i < _views.size(); i++) {
			_views[i]->initialize();
			//TODO: Refactor the whole TextComponent. Allignment works incorrectly!
			//_views[i]->getEntity()->getComponent<TextComponent>()->setAllignment({TextAllignment::CENTER, TextAllignment::CENTER});
			auto mapText = std::make_shared<ExpressionOperation>();
			//NOTE: we send list without index only because we use SIZE
			mapText->addArgument(_views[0]->getEntity(), "CustomStringArrayComponent", "list0");
			mapText->addArgument(nullptr, "", std::to_string(i));
			mapText->addArgument(_views[0]->getEntity(), "CustomFloatComponent", "listStartVertical");
			mapText->addArgument(_views[i]->getEntity(), "TextComponent", "focus");
			mapText->initializeOperation("${1} + ${2} < SIZE ${0} AND ${3} = 0");
			auto setLine = std::make_shared<AssignAction>();
			setLine->addArgument(_views[i]->getEntity(), "TextComponent", "text");
			//TODO: need to take every column from list and map to appropriate text in view
			setLine->addArgument(_views[0]->getEntity(), "CustomStringArrayComponent", "list0");
			setLine->addArgument(_views[0]->getEntity(), "CustomFloatComponent", "listStartVertical");
			setLine->addArgument(nullptr, "", std::to_string(i));
			setLine->initializeAction("${0} SET ${1} AT ( ${2} + ${3} )");
			mapText->registerAction(setLine);
			//TODO: add common ONCE + mouse/keyboard
			_views[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(mapText, InteractionType::COMMON_END);
		}
	}
	return false;
}

ListFactory::ListFactory(std::shared_ptr<Scene> activeScene, std::shared_ptr<ViewFactory> itemFactory) {
	_activeScene = activeScene;
	_itemFactory = itemFactory;
}

std::shared_ptr<View> ListFactory::createView(std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<List> list = std::make_shared<List>(name);
	list->setParent(parent);

	for (int i = 0; i < 4; i++) {
		list->addView(_itemFactory->createView("Item"+std::to_string(i), list));
	}
	return list;
}

std::shared_ptr<View> ListFactory::createGrid(std::tuple<int, int> dim, std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<List> list = std::make_shared<List>(name);
	list->setParent(parent);

	for (int i = 0; i < 4; i++) {
		list->addView(_itemFactory->createGrid(dim, "Item" + std::to_string(i), list));
	}
	return list;
}