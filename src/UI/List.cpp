#include "List.h"
#include "GraphicComponents.h"

List::List(std::string name, std::shared_ptr<ViewFactory> itemFactory) {
	_viewName = name;
	_itemFactory = itemFactory;
}

bool List::initialize() {
	getBack()->initialize();
	getBack()->setColorMask({ 0, 0, 0, 0 });
	getBack()->setColorAddition({ 1, 0, 1, 1 });
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

bool List::addItem(std::string text) {
	auto view = _itemFactory->createView();
	std::tuple<float, float> startPosition = getBack()->getPosition();
	std::tuple<float, float> size = {};
	if (_views.size() > 1)
		startPosition = _views.back()->getEntity()->getComponent<ObjectComponent>()->getPosition();
	view->getEntity()->getComponent<TextComponent>()->setText(text);
	_views.push_back(view);
	return false;
}

ListFactory::ListFactory(std::shared_ptr<Scene> activeScene, std::shared_ptr<ViewFactory> itemFactory) {
	_activeScene = activeScene;
	_itemFactory = itemFactory;
	_backFactory = std::make_shared<BackFactory>(activeScene);
}

std::shared_ptr<View> ListFactory::createView(std::string name, std::shared_ptr<View> parent) {
	std::shared_ptr<List> list = std::make_shared<List>(name, _itemFactory);
	list->setBack(_backFactory->createView());
	return list;
}