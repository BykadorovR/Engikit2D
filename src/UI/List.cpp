#include "List.h"
#include "GraphicComponents.h"
#include "CustomComponents.h"
#include "InteractionOperations.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"
#include "UserInputComponents.h"

//TODO: add header to list for specifiying entity name/ID, component's name/ID
List::List(std::string name) {
	_viewName = name;
}

bool List::setEditable(bool editable) {
	for (int i = 1; i < _views.size(); i++) {
		std::dynamic_pointer_cast<Label>(_views[i])->setEditable(editable);
	}
	return false;
}

bool List::setPosition(std::tuple<float, float> position) {
	std::tuple<float, float> listItemSize = _views[0]->getEntity()->getComponent<ObjectComponent>()->getSize();
	_views[0]->getEntity()->getComponent<ObjectComponent>()->setMember("positionX", std::get<0>(position));
	_views[0]->getEntity()->getComponent<ObjectComponent>()->setMember("positionY", std::get<1>(position));
	for (int i = 1; i < _views.size(); i++) {
		auto prevPosition = _views[i - 1]->getEntity()->getComponent<ObjectComponent>()->getPosition();
		std::tuple<float, float> currentPosition = { std::get<0>(prevPosition), std::get<1>(prevPosition) + std::get<1>(listItemSize) };

		_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("positionX", std::get<0>(currentPosition));
		_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("positionY", std::get<1>(currentPosition));
	}
	return false;
}

bool List::setSize(std::tuple<float, float> size) {
	std::tuple<float, float> listItemSize = { std::get<0>(size), std::get<1>(size) / _views.size() };
	for (int i = 0; i < _views.size(); i++) {
		_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("sizeX", std::get<0>(listItemSize));
		_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("sizeY", std::get<1>(listItemSize));
	}
	return false;
}

bool List::addItem(std::string text) {
	_views[0]->getEntity()->getComponent<CustomStringArrayComponent>()->addCustomValue(text, "list");
	return false;
}

bool List::clear() {
	_views[0]->getEntity()->getComponent<CustomStringArrayComponent>()->clear("list");
	for (int i = 0; i < _views.size(); i++)
		_views[i]->getEntity()->getComponent<TextComponent>()->setText("");
	return false;
}

//views and items are different thigs. Views - labels, items - string (text)
bool List::initialize() {
	_views[0]->getEntity()->createComponent<CustomFloatComponent>()->addCustomValue("listStartPage", 0);
	_views[0]->getEntity()->createComponent<CustomStringArrayComponent>()->initializeEmpty("list");
	for (int i = 0; i < _views.size(); i++) {
		_views[i]->initialize();
		//TODO: Refactor the whole TextComponent. Allignment works incorrectly!
		//_views[i]->getEntity()->getComponent<TextComponent>()->setAllignment({TextAllignment::CENTER, TextAllignment::CENTER});
		auto mapText = std::make_shared<ExpressionOperation>();
		//NOTE: we send list without index only because we use SIZE
		mapText->addArgument(_views[0]->getEntity(), "CustomStringArrayComponent", "list");
		mapText->addArgument(nullptr, "", std::to_string(i));
		mapText->addArgument(_views[0]->getEntity(), "CustomFloatComponent", "listStartPage");
		mapText->addArgument(_views[i]->getEntity(), "TextComponent", "focus");
		mapText->initializeOperation("${1} + ${2} < SIZE ${0} AND ${3} = 0");
		auto setLine = std::make_shared<AssignAction>();
		setLine->addArgument(_views[i]->getEntity(), "TextComponent", "text");
		setLine->addArgument(_views[0]->getEntity(), "CustomStringArrayComponent", "list");
		setLine->addArgument(_views[0]->getEntity(), "CustomFloatComponent", "listStartPage");
		setLine->addArgument(nullptr, "", std::to_string(i));
		setLine->initializeAction("${0} SET ${1} AT ( ${2} + ${3} )");
		mapText->registerAction(setLine);
		//TODO: add common ONCE + mouse/keyboard
		_views[0]->getEntity()->createComponent<InteractionComponent>()->attachOperation(mapText, InteractionType::COMMON_END);
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