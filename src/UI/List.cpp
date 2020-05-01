#include "List.h"
#include "GraphicComponents.h"
#include "CustomComponents.h"
#include "InteractionOperations.h"
#include "InteractionActions.h"
#include "InteractionComponents.h"
#include "UserInputComponents.h"

List::List(std::string name) {
	_viewName = name;
}

bool List::setPosition(std::tuple<float, float> postion) {
	getBack()->setPosition(postion);
	auto listSize = getBack()->getSize();
	std::tuple<float, float> listItemSize = { std::get<0>(listSize), std::get<1>(listSize) / (_views.size() - 1) };
	for (int i = 2; i < _views.size(); i++) {
		auto prevPosition = _views[i - 1]->getEntity()->getComponent<ObjectComponent>()->getPosition();
		std::tuple<float, float> currentPosition = { std::get<0>(prevPosition), std::get<1>(prevPosition) + std::get<1>(listItemSize) };

		_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("positionX", std::get<0>(currentPosition));
		_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("positionY", std::get<1>(currentPosition));
	}
	return false;
}

bool List::setSize(std::tuple<float, float> size) {
	getBack()->setSize(size);
	auto listSize = getBack()->getSize();
	std::tuple<float, float> listItemSize = { std::get<0>(listSize), std::get<1>(listSize) / (_views.size() - 1) };
	for (int i = 1; i < _views.size(); i++) {
		_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("sizeX", std::get<0>(listItemSize));
		_views[i]->getEntity()->getComponent<ObjectComponent>()->setMember("sizeY", std::get<1>(listItemSize));
	}
	return false;
}

bool List::addItem(std::string text) {
	getBack()->getEntity()->getComponent<CustomStringArrayComponent>()->addCustomValue(text, "list");
	return false;
}

bool List::initialize() {
	getBack()->initialize();
	getBack()->setColorMask({ 0, 0, 0, 0 });
	getBack()->setColorAddition({ 1, 0, 1, 1 });
	getBack()->getEntity()->createComponent<KeyboardComponent>();
	
	getBack()->getEntity()->getComponent<CustomFloatComponent>()->addCustomValue(0, "page");

	for (int i = 1; i < _views.size(); i++) {
		_views[i]->initialize();
		_views[i]->getEntity()->getComponent<TextComponent>()->setAllignment({TextAllignment::CENTER, TextAllignment::CENTER});
		auto mapText = std::make_shared<ExpressionOperation>();
		//NOTE: we send list without index only because we use SIZE
		mapText->addArgument(getBack()->getEntity()->getComponent<CustomStringArrayComponent>(), "list");
		mapText->addArgument(nullptr, std::to_string(i - 1), -1);
		mapText->addArgument(getBack()->getEntity()->getComponent<CustomFloatComponent>(), "page");
		mapText->initializeOperation("SIZE ${0} > ${1} + ${2}");
		getBack()->getEntity()->createComponent<InteractionComponent>()->attachOperation(mapText, InteractionType::COMMON_END);
		auto setLine = std::make_shared<AssignAction>();
		setLine->addArgument(_views[i]->getEntity()->getComponent<TextComponent>(), "text");
		setLine->addArgument(getBack()->getEntity()->getComponent<CustomStringArrayComponent>(), "list", i - 1);
		setLine->initializeAction("${0} SET ${1}");
		mapText->registerAction(setLine);
	}
	
	setPosition(getBack()->getPosition());
	setSize(getBack()->getSize());

	return false;
}

std::shared_ptr<Back> List::getBack() {
	for (auto view : _views) {
		if (view->getName() == "Back")
			return std::dynamic_pointer_cast<Back>(view);
	}
	return nullptr;
}

bool List::setBack(std::shared_ptr<View> back) {
	_views.push_back(back);
	return false;
}

bool List::addView(std::shared_ptr<View> view) {
	_views.push_back(view);
	return false;
}

//assume that back is always on top
std::vector<std::shared_ptr<View> > List::getViews() {
	return std::vector<std::shared_ptr<View> >(_views.begin() + 1, _views.end());
}

ListFactory::ListFactory(std::shared_ptr<Scene> activeScene, std::shared_ptr<ViewFactory> itemFactory) {
	_activeScene = activeScene;
	_itemFactory = itemFactory;
	_backFactory = std::make_shared<BackFactory>(activeScene);
}

std::shared_ptr<View> ListFactory::createView(std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<List> list = std::make_shared<List>(name);
	auto back = _backFactory->createView();
	back->getEntity()->createComponent<CustomStringArrayComponent>();
	back->getEntity()->createComponent<CustomFloatComponent>();
	list->setBack(back);
	int listItems = 3;
	for (int i = 0; i < listItems; i++) {
		list->addView(_itemFactory->createView());
	}
	
	return list;
}