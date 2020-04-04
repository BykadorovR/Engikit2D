#pragma once
#include "Action.h"
#include "Expression.h"
#include "Scene.h"
#include "Decorator.h"


class AssignAction : public Action {
private:
	std::shared_ptr<Expression> _expression;
	std::vector<std::string> _postfix;
	std::map<std::string, std::tuple<int, std::string> > _supportedOperations;
	std::vector<std::tuple<std::shared_ptr<Component>, std::string> > _arguments;
public:
	AssignAction();
	bool addArgument(std::shared_ptr<Component> argument, std::string name);
	
	bool initializeAction(std::string condition);
	bool doAction();
};

class LabelDecoratorDoAction : public Action {
private:
	std::shared_ptr<Scene> _activeScene;
	//TODO: extend for any decorator
	std::shared_ptr<ScrollerDecorator> _decorator;
	std::shared_ptr<Label> _label;
public:
	LabelDecoratorDoAction();
	
	bool initializeAction(std::shared_ptr<ScrollerDecorator> decorator, std::shared_ptr<Scene> activeScene);
	bool doAction();
};

class LabelDecoratorUndoAction : public Action {
private:
	std::shared_ptr<Scene> _activeScene;
	//TODO: extend for any decorator
	std::shared_ptr<ScrollerDecorator> _decorator;
	std::shared_ptr<Label> _label;
public:
	LabelDecoratorUndoAction();

	bool initializeAction(std::shared_ptr<ScrollerDecorator> decorator, std::shared_ptr<Scene> activeScene);
	bool doAction();
};