#include "Scene.h"
#include "View.h"
#include "Label.h"

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

class MainInterface {
private:
	std::shared_ptr<ViewDecorators> _viewDecorators;
public:
	MainInterface();
	bool initialize(std::shared_ptr<Scene> scene);
};