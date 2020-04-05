#pragma once
#include "System.h"
#include "InteractionComponents.h"

class InteractionSystem : public System {
private:

public:
	InteractionSystem();
	// Called every game update
	void update(InteractionType type);
};