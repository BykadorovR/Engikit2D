#include "Scene.h"
#include "View.h"
#include "Label.h"
#include "Decorator.h"
#include "List.h"
#include "State.h"

class ViewDecorators {
private:
	std::vector<std::shared_ptr<ViewFactory> > _factories;
public:
	bool initialize(std::shared_ptr<Scene> scene);
	template<class T>
	std::shared_ptr<ViewFactory> getFactory() {
		for (auto factory : _factories) {
			if (std::dynamic_pointer_cast<T>(factory)) {
				return factory;
			}
		}
	}
};

class ComplexList {
private:
	std::string _name;
	std::shared_ptr<ViewDecorators> _viewDecorators;

	std::vector<std::shared_ptr<Back> > _back;
	std::shared_ptr<List> _list;
	std::shared_ptr<ScrollerVerticalDecorator> _verticalScrollerDecorator;
	std::shared_ptr<HeaderDecorator> _headerDecorator;
public:
	ComplexList() = default;
	bool initialize(std::shared_ptr<ViewDecorators> viewDecorators);

	std::vector<std::shared_ptr<Back> > getBack();
	std::shared_ptr<List> getList();

	bool setPosition(std::tuple<int, int> position);
	bool setSize(std::tuple<int, int> size);
	std::tuple<int, int> getSize();
	std::tuple<int, int> getPosition();

	std::string getName();
	bool setName(std::string name);
};

class MainInterface {
private:
	std::shared_ptr<ViewDecorators> _viewDecorators;
	std::tuple<int, int> _listItemSize = { 150, 100 };

	std::shared_ptr<ComplexList> _componentsList;
	std::shared_ptr<ComplexList> _fieldsList;
public:
	MainInterface() = default;
	bool initialize(std::shared_ptr<Scene> scene);
};