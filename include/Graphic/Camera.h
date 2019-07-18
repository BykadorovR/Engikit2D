#pragma once
#include "Entity.h"
#include "Component.h"
#include "Matrix.h"

class CameraComponent : public Component, IMouseEvent {
public:
	void initialize(int entityID, GLuint program) {
		_entityID = entityID;
		_uViewMatrixLocation = glGetUniformLocation(program, _uViewMatrix.c_str());
		MouseEvent::instance().registerComponent(this);
	}

	~CameraComponent() {
		MouseEvent::instance().unregisterComponent(this);
	}

	void setTransform(std::tuple<float, float> coords) {
		_coords = coords;
	}

	void mouseClickDownLeft(int x, int y) {
		_leftClick = { x, y };
	}
	void mouseClickDownRight(int x, int y) {
		_rightClick = { x, y };
	}

	bool _move = false;
	std::tuple<float, float> _leftClick;
	std::tuple<float, float> _rightClick;
	std::tuple<float, float> _coords;
	int _entityID;
	GLuint _program;
	//
	GLint _uViewMatrixLocation;
	//
	std::string _uViewMatrix = "u_ViewMatrix";
};