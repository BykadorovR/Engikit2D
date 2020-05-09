#pragma once
#include "Action.h"
#include "Entity.h"
#include "List.h"
#include "GraphicComponents.h"

class PrintComponentsAction : public Action {
private:
	std::shared_ptr<Entity> _entity;
	std::shared_ptr<List> _list;
public:
	PrintComponentsAction();
	bool doAction();
	bool setList(std::shared_ptr<List> list);
	bool setEntity(std::shared_ptr<Entity> entity);
};

class ClearComponentsAction : public Action {
private:
	std::shared_ptr<Entity> _entity;
	std::shared_ptr<List> _list;
public:
	ClearComponentsAction();
	bool doAction();
	bool setList(std::shared_ptr<List> list);
	bool setEntity(std::shared_ptr<Entity> entity);
};

class PrintItemsAction : public Action {
private:
	std::shared_ptr<TextComponent> _component;
	std::shared_ptr<Entity> _entity;
	std::shared_ptr<List> _list;
public:
	PrintItemsAction();
	bool doAction();
	bool setList(std::shared_ptr<List> list);
	bool setComponent(std::shared_ptr<TextComponent> component);
	bool setEntity(std::shared_ptr<Entity> entity);
};

class ApplyItemAction : public Action {
private:
	std::shared_ptr<OperationComponent> _component;
	int _viewIndex;
	std::shared_ptr<List> _list;
public:
	ApplyItemAction();
	bool doAction();
	bool setList(std::shared_ptr<List> view);
	bool setComponent(std::shared_ptr<OperationComponent> component);
	bool setViewIndex(int viewIndex);
};