#include "Entity.h"
#include "Component.h"
#include "ComponentFunctors.h"
#include "Matrix.h"

class CameraComponent : public Component {
public:
	void initialize(int entityID) {
		_entityID = entityID;
		_viewMatrix.identity();
	}

	void setTransform(GLuint program, std::tuple<float, float> coords) {
		_uViewMatrixLocation = glGetUniformLocation(program, _uViewMatrix.c_str());
		_viewMatrix.translate(std::get<0>(coords), std::get<1>(coords));
		glUniformMatrix4fv(_uViewMatrixLocation, 1, false, _viewMatrix.getData());
	}

	int _entityID;
	Matrix2D _viewMatrix;
	//
	GLint _uViewMatrixLocation;
	//
	std::string _uViewMatrix = "u_ViewMatrix";
};