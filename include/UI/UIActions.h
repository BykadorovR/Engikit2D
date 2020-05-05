#pragma once
#include "Action.h"
#include "Entity.h"
#include "List.h"

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
