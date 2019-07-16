#pragma once
#include "Entity.h"
#include "Component.h"
#include "Matrix.h"

class CameraComponent : public Component {
public:
	void initialize(int entityID, GLuint program) {
		_entityID = entityID;
		_uViewMatrixLocation = glGetUniformLocation(program, _uViewMatrix.c_str());
		_viewMatrix.identity();
	}

	int _entityID;
	GLuint _program;
	Matrix2D _viewMatrix;
	//
	GLint _uViewMatrixLocation;
	//
	std::string _uViewMatrix = "u_ViewMatrix";
};