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

	void setTransform(std::tuple<float, float> coords) {
		Matrix2D move;
		float x = -std::get<0>(coords);
		float y = -std::get<1>(coords);
		move.translate(x, y);
		_viewMatrix = _viewMatrix * move;
		glUniformMatrix4fv(_uViewMatrixLocation, 1, false, _viewMatrix.getData());
	}

	int _entityID;
	GLuint _program;
	Matrix2D _viewMatrix;
	//
	GLint _uViewMatrixLocation;
	//
	std::string _uViewMatrix = "u_ViewMatrix";
};