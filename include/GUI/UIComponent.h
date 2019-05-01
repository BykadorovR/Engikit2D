#pragma once
#include "Component.h"
#include "Common.h"
#include "Window.h"
#include "Events.h"

class UserInteractionComponent : public Component, IMouseEvent {
public:
	void initialize() {
		MouseEvent::instance().registerComponent(this);
	}

	void mouseClickDownLeft(int x, int y) {

	}
	void mouseClickDownRight(int x, int y) {

	}
	void mouseMove(int x, int y) {

	}
};