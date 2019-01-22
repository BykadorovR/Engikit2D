#include "Texture.h"
#include "Buffer.h"
#include "Shader.h"

class Sprite {
public:
	Sprite(float _sceneX, float _sceneY, float _spriteWidth, float _spriteHeight, Texture _texture);
	void draw();
	void attach();
	void attachHUD();
	void changeTexture(Texture texture);
	Texture getTexture();
private:
	float sceneX, sceneY;
	float spriteWidth, spriteHeight;
	Texture texture;
	//
	Buffer textureBuffer;
	Shader shader;
	//
	GLuint program;
	GLuint textureID;
	GLint aPositionLocation;
	GLint aTextureCoordinatesLocation;
	GLint uTextureUnitLocation;
	std::string aPositionString = "a_Position";
	std::string aTextureCoordinatesString = "a_TextureCoordinates";
	std::string uTextureUnitString = "u_TextureUnit";
	//should be separated from attach logic to be able change some stuff, for example, texture
	void initializeBuffer();
};