#pragma once
#include "EntityManager.h"

class System {
protected:
	bool _editorMode;
	std::shared_ptr<EntityManager> _entityManager;
public:
	System();
	// Called every game update
	void setEntityManager(std::shared_ptr<EntityManager> entityManager);
	virtual ~System() = default;
};