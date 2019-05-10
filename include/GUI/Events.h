#pragma once
#include <iostream>
#include "PlatformGL.h"
#include <vector>

//TODO: Pattern observed, all components should be subscribed to Mouse class
//Move implementation to respective cpp (Bridge pattern) (defines or separate files?)
//MouseEvent class should be singleton

//These methods are needed cause we should send callback to glut but we can't use classes' methods
void mousePress(int button, int state, int x, int y);

//Interface that should be inherited by classes that want handle mouse events
class IMouseEvent {
public:
	virtual void mouseClickDownLeft(int x, int y) = 0;
	virtual void mouseClickDownRight(int x, int y) = 0;
};


//Singleton
class MouseEvent {
private:

	std::vector<IMouseEvent* > _listeners;
public:
	static MouseEvent& instance() {
		static MouseEvent object;
		return object;
	}

	void registerComponent(IMouseEvent* listener);
	void unregisterComponent(IMouseEvent* listener);

	void mousePress(int button, int state, int x, int y);
};