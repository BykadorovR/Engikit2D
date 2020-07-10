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

	std::vector<std::shared_ptr<Grid> > _back;
	std::shared_ptr<List> _list;
	std::shared_ptr<ScrollerVerticalDecorator> _verticalScrollerDecorator;
	std::shared_ptr<HeaderDecorator> _headerDecorator;
public:
	ComplexList() = default;
	bool initialize(std::tuple<int, int> dim, std::shared_ptr<ViewDecorators> viewDecorators);

	std::vector<std::shared_ptr<Grid> > getBack();
	std::shared_ptr<List> getList();
	std::shared_ptr<HeaderDecorator> getHeaderDecorator();
	std::shared_ptr<ScrollerVerticalDecorator> getVerticalScrollerDecorator();

	bool setPosition(std::tuple<float, float> position);
	bool setSize(std::tuple<std::vector<float>, float> size);
	bool setHeader(std::vector<std::string> text);

	std::tuple<std::vector<float>, float> getSize();
	std::tuple<float, float> getComplexSize();
	std::tuple<float, float> getPosition();
	std::tuple<float, float> getComplexPosition();

	std::string getName();
	bool setName(std::string name);
};

class ComplexLabel {
private:
	std::string _name;

	std::shared_ptr<Back> _back;
	std::shared_ptr<Label> _label;
	std::shared_ptr<HeaderDecorator> _headerDecorator;
public:
	ComplexLabel() = default;
	bool initialize(std::shared_ptr<ViewDecorators> viewDecorators);

	std::tuple<float, float> getSize();
	std::tuple<float, float> getComplexSize();
	std::tuple<float, float> getPosition();
	std::tuple<float, float> getComplexPosition();

	std::shared_ptr<Back> getBack();
	std::shared_ptr<Label> getLabel();
	std::shared_ptr<HeaderDecorator> getHeaderDecorator();

	bool setPosition(std::tuple<float, float> position);
	bool setSize(std::tuple<float, float> size);
	bool setHeader(std::vector<std::string> text);
	bool setText(std::string text);

	std::string getName();
	bool setName(std::string name);
};

class MainInterface {
private:
	std::shared_ptr<ViewDecorators> _viewDecorators;
	std::shared_ptr<ComplexList> _entitiesList;

	std::shared_ptr<ComplexList> _componentsList;
	std::shared_ptr<ComplexList> _fieldsList;
	std::shared_ptr<ComplexList> _operationsList;
	std::shared_ptr<ComplexList> _commandsList;
	std::shared_ptr<ComplexList> _argumentsList;
	std::shared_ptr<ComplexList> _resourcesList;
	std::shared_ptr<ComplexList> _scenesList;
	std::shared_ptr<ComplexLabel> _argumentTypeLabel;
public:
	MainInterface() = default;
	bool initialize(std::shared_ptr<Scene> scene);
	bool fillEntitiesList(std::shared_ptr<Scene> scene);
};