#include "Texture.h"
#include "Buffer.h"
#include "Shader.h"
#include "Matrix.h"

class Object {
public:
	Object(int sceneX, int sceneY, int objectWidth, int objectHeight, Texture texture);
	virtual void draw();
	virtual void attach();
	void attachHUD();
	void translate(int posX, int posY);
	int getX();
	int getY();
	int getWidth();
	int getHeight();
protected:
	//should be separated from attach logic to be able change some stuff, for example, texture
	virtual void setBuffer();
	//
	int _sceneX, _sceneY;
	int _objectWidth, _objectHeight;
	Texture _texture;
	//
	Buffer _bufferVertex;
	Buffer _bufferTexture;
	Shader _shader;
	Matrix2D _transform;
	//
	GLuint _program;
	GLuint _textureID;
	GLint _aPositionLocation;
	GLint _aTextureCoordinatesLocation;
	GLint _uTextureUnitLocation;
	GLint _uMatrixLocation;
	//
	std::string _aPositionString = "a_Position";
	std::string _aTextureCoordinatesString = "a_TextureCoordinates";
	std::string _uTextureUnitString = "u_TextureUnit";
	std::string _uMatrix = "u_Matrix";
};