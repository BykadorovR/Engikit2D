#include "System.h"

System::System() {
	_editorMode = true;
}

void System::setEntityManager(std::shared_ptr<EntityManager> entityManager) {
	_entityManager = entityManager;
}