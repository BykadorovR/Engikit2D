#pragma once
#include <iostream>
#include "PlatformGL.h"
#include <vector>

//TODO: Pattern observed, all components should be subscribed to Mouse class
//Move implementation to respective cpp (Bridge pattern) (defines or separate files?)
//MouseEvent class should be singleton

class IMouseEvent {
public:
	virtual void mouseClickDownLeft(int x, int y) = 0;
	virtual void mouseClickDownRight(int x, int y) = 0;
	virtual void mouseMove(int x, int y) = 0;
};


class MouseEvent {
private:
	MouseEvent();

	std::vector<std::shared_ptr<IMouseEvent> > _listeners;
public:
	static MouseEvent& initialize() {
		static MouseEvent instance;
		return instance;
	}

	void registerComponent(std::shared_ptr<IMouseEvent> listener);

	void mouseCursorPassive(int x, int y);

	void mousePress(int button, int state, int x, int y);
};