#pragma once
#include "System.h"
#include "InteractionComponents.h"

class InteractionSystem : public System {
public:
	// Called every game update
	void update(InteractionType type);
};

class StateSystem : public System {
public:
	//
	void update(InteractionType type);
};