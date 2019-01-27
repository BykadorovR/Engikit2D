#include "Texture.h"
#include "Buffer.h"
#include "Shader.h"
#include "Matrix.h"

class Sprite {
public:
	Sprite(int _sceneX, int _sceneY, int _spriteWidth, int _spriteHeight, Texture _texture);
	void draw();
	void attach();
	void translate(int posX, int posY);
	void attachHUD();
	void changeTexture(Texture texture);
	Texture getTexture();
private:
	int sceneX, sceneY;
	int spriteWidth, spriteHeight;
	Texture texture;
	//
	Buffer spriteBuffer;
	Shader shader;
	Matrix2D transform;
	//
	GLuint program;
	GLuint textureID;
	GLint aPositionLocation;
	GLint aTextureCoordinatesLocation;
	GLint uTextureUnitLocation;
	GLint uMatrixLocation;
	std::string aPositionString = "a_Position";
	std::string aTextureCoordinatesString = "a_TextureCoordinates";
	std::string uTextureUnitString = "u_TextureUnit";
	std::string uMatrix = "u_Matrix";
	//should be separated from attach logic to be able change some stuff, for example, texture
	void initializeBuffer();
};