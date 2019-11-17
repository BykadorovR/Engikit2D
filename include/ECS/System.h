#pragma once
#include "EntityManager.h"

class System {
private:
	bool _editorMode;
public:
	System();
	// Called every game update
	virtual void update(std::shared_ptr<EntityManager> entityManager);

};