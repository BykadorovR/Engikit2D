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
	std::vector<std::shared_ptr<Back> > _back;
	std::shared_ptr<List> _list;
	std::shared_ptr<ScrollerVerticalDecorator> _verticalScrollerDecorator;
	std::shared_ptr<HeaderDecorator> _headerDecorator;
public:
	ComplexList() = default;
	bool initialize(std::tuple<float, float> position, std::tuple<int, int> itemSize, std::shared_ptr<ViewDecorators> viewDecorators);
};

class MainInterface {
private:
	std::shared_ptr<ViewDecorators> _viewDecorators;
public:
	MainInterface() = default;
	bool initialize(std::shared_ptr<Scene> scene);
};