#pragma once
#include "Component.h"
#include "Common.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Matrix.h"
#include "Events.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class ObjectComponent : public Component {
public:
	void initialize(int sceneX, int sceneY, int objectWidth, int objectHeight, GLuint program) {
		_program = program;
		_objectWidth = objectWidth;
		_objectHeight = objectHeight;
		_sceneX = sceneX;
		_sceneY = sceneY;

		float objectWidthN = (float)_objectWidth / (float)resolution.first;
		float objectHeightN = (float)_objectHeight / (float)resolution.second;
		float startX = (float)(_sceneX) / (float)resolution.first;
		float startY = (float)(resolution.second - _sceneY) / (float)resolution.second;
		// Order of coordinates: X, Y
		// 0   2
		// | / |
		// 1   3
		float vertexData[] = { startX,                startY,
							   startX,                startY - objectHeightN,
							   startX + objectWidthN, startY,
							   startX + objectWidthN, startY - objectHeightN };
		assert(_buffer.bindVBO(vertexData, sizeof(vertexData), GL_STATIC_DRAW) == TW_OK);
		_aPositionLocation = glGetAttribLocation(_program, _aPositionString.c_str());
	}
	Matrix2D _transform;
	Matrix2D _camera;
	float _cameraCoefSpeed;
	//
	float _sceneX, _sceneY;
	int _objectWidth, _objectHeight;
	//
	Buffer _buffer;
	//
	GLint _aPositionLocation;
	GLuint _program;
	//
	std::string _aPositionString = "a_Position";
};

class TextureComponent : public Component {
public:
	void initialize(int textureID, GLuint program) {
		_componentID = 1;
		_solid = 0;
		_texture = TextureManager::instance()->getTexture(textureID);
		_program = program;
		float posXInAtlasN = (float)_texture->getX() / (float)_texture->getAtlas()->getWidth();
		float posYInAtlasN = (float)_texture->getY() / (float)_texture->getAtlas()->getHeight();
		float textureWidthN = (float)_texture->getWidth() / (float)_texture->getAtlas()->getWidth();
		float textureHeightN = (float)_texture->getHeight() / (float)_texture->getAtlas()->getHeight();
		// Order of coordinates: S, T
		// 0   2
		// | / |
		// 1   3
		float textureData[] = { posXInAtlasN,                 posYInAtlasN,
								posXInAtlasN,                 posYInAtlasN + textureHeightN,
								posXInAtlasN + textureWidthN, posYInAtlasN,
								posXInAtlasN + textureWidthN, posYInAtlasN + textureHeightN };
		assert(_buffer.bindVBO(textureData, sizeof(textureData), GL_STATIC_DRAW) == TW_OK);
		_aTextureCoordinatesLocation = glGetAttribLocation(_program, _aTextureCoordinatesString.c_str());
		_uTextureUnitLocation = glGetUniformLocation(_program, _uTextureUnitString.c_str());
		_uAdjustXLocation = glGetUniformLocation(_program, _uAdjustX.c_str());
		_uAdjustYLocation = glGetUniformLocation(_program, _uAdjustY.c_str());
		_uSolidLocation = glGetUniformLocation(_program, _uSolidString.c_str());
		_textureObject = _texture->getAtlas()->getTexureObjectID();

	}

	void initialize(std::shared_ptr<Texture> texture, GLuint program) {
		_componentID = 1;
		_solid = 0;
		_texture = texture;
		_program = program;
		float posXInAtlasN = (float)_texture->getX() / (float)_texture->getAtlas()->getWidth();
		float posYInAtlasN = (float)_texture->getY() / (float)_texture->getAtlas()->getHeight();
		float textureWidthN = (float)_texture->getWidth() / (float)_texture->getAtlas()->getWidth();
		float textureHeightN = (float)_texture->getHeight() / (float)_texture->getAtlas()->getHeight();
		// Order of coordinates: S, T
		// 0   2
		// | / |
		// 1   3
		float textureData[] = { posXInAtlasN,                 posYInAtlasN,
								posXInAtlasN,                 posYInAtlasN + textureHeightN,
								posXInAtlasN + textureWidthN, posYInAtlasN,
								posXInAtlasN + textureWidthN, posYInAtlasN + textureHeightN };
		assert(_buffer.bindVBO(textureData, sizeof(textureData), GL_STATIC_DRAW) == TW_OK);
		_aTextureCoordinatesLocation = glGetAttribLocation(_program, _aTextureCoordinatesString.c_str());
		_uTextureUnitLocation = glGetUniformLocation(_program, _uTextureUnitString.c_str());
		_uAdjustXLocation = glGetUniformLocation(_program, _uAdjustX.c_str());
		_uAdjustYLocation = glGetUniformLocation(_program, _uAdjustY.c_str());
		_uSolidLocation = glGetUniformLocation(_program, _uSolidString.c_str());
		_textureObject = texture->getAtlas()->getTexureObjectID();

	}

	void initialize(GLuint program) {
		_componentID = 1;
		_solid = 1;
		_texture = nullptr;
		_program = program;
		_uSolidLocation = glGetUniformLocation(_program, _uSolidString.c_str());
	}

	int _solid = 1;
	//
	Buffer _buffer;
	std::shared_ptr<Texture> _texture;
	//
	GLuint _program;
	//atlas ID
	GLuint _textureObject;
	GLint _aTextureCoordinatesLocation;
	GLint _uTextureUnitLocation;
	GLint _uAdjustXLocation;
	GLint _uAdjustYLocation;
	GLint _uSolidLocation;
	//
	std::string _aTextureCoordinatesString = "a_TextureCoordinates";
	std::string _uTextureUnitString = "u_TextureUnit";
	std::string _uSolidString = "u_Solid";
	std::string _uAdjustX = "u_AdjustX";
	std::string _uAdjustY = "u_AdjustY";

};

class AnimatedTextureComponent : public Component {
public:
	void initialize(int textureID, std::vector<int> tilesOrder, std::vector<int> tilesLatency, GLuint program) {
		_componentID = 1;
		_solid = 0;
		_texture = TextureManager::instance()->getTexture(textureID);
		_program = program;
		_tilesLatency = tilesLatency;
		_tilesOrder = tilesOrder;
		float posXInAtlasN = (float)_texture->getX() / (float)_texture->getAtlas()->getWidth();
		float posYInAtlasN = (float)_texture->getY() / (float)_texture->getAtlas()->getHeight();
		float widthTile = (float)_texture->getWidth() / (float)_texture->getColumn() / (float)_texture->getAtlas()->getWidth();
		_widthTile = widthTile;
		float heightTile = (float)_texture->getHeight() / (float)_texture->getRow() / (float)_texture->getAtlas()->getHeight();
		_heightTile = heightTile;
		// Order of coordinates: S, T
		// 0   2
		// | / |
		// 1   3
		float textureData[] = { posXInAtlasN,                 posYInAtlasN,
								posXInAtlasN,                 posYInAtlasN + heightTile,
								posXInAtlasN + widthTile, posYInAtlasN,
								posXInAtlasN + widthTile, posYInAtlasN + heightTile };
		assert(_buffer.bindVBO(textureData, sizeof(textureData), GL_STATIC_DRAW) == TW_OK);
		_aTextureCoordinatesLocation = glGetAttribLocation(_program, _aTextureCoordinatesString.c_str());
		_uTextureUnitLocation = glGetUniformLocation(_program, _uTextureUnitString.c_str());
		_uAdjustXLocation = glGetUniformLocation(_program, _uAdjustX.c_str());
		_uAdjustYLocation = glGetUniformLocation(_program, _uAdjustY.c_str());
		_uSolidLocation = glGetUniformLocation(_program, _uSolidString.c_str());
		_textureObject = _texture->getAtlas()->getTexureObjectID();

	}
	void initialize(std::shared_ptr<Texture> texture, std::vector<int> tilesOrder, std::vector<int> tilesLatency, GLuint program) {
		_componentID = 1;
		_solid = 0;
		_texture = texture;
		_program = program;
		_tilesLatency = tilesLatency;
		_tilesOrder = tilesOrder;
		float posXInAtlasN = (float)_texture->getX() / (float)_texture->getAtlas()->getWidth();
		float posYInAtlasN = (float)_texture->getY() / (float)_texture->getAtlas()->getHeight();
		float widthTile = (float)_texture->getWidth() / (float)_texture->getColumn() / (float)_texture->getAtlas()->getWidth();
		_widthTile = widthTile;
		float heightTile = (float)_texture->getHeight() / (float)_texture->getRow() / (float)_texture->getAtlas()->getHeight();
		_heightTile = heightTile;
		// Order of coordinates: S, T
		// 0   2
		// | / |
		// 1   3
		float textureData[] = { posXInAtlasN,                 posYInAtlasN,
								posXInAtlasN,                 posYInAtlasN + heightTile,
								posXInAtlasN + widthTile, posYInAtlasN,
								posXInAtlasN + widthTile, posYInAtlasN + heightTile };
		assert(_buffer.bindVBO(textureData, sizeof(textureData), GL_STATIC_DRAW) == TW_OK);
		_aTextureCoordinatesLocation = glGetAttribLocation(_program, _aTextureCoordinatesString.c_str());
		_uTextureUnitLocation = glGetUniformLocation(_program, _uTextureUnitString.c_str());
		_uAdjustXLocation = glGetUniformLocation(_program, _uAdjustX.c_str());
		_uAdjustYLocation = glGetUniformLocation(_program, _uAdjustY.c_str());
		_uSolidLocation = glGetUniformLocation(_program, _uSolidString.c_str());
		_textureObject = texture->getAtlas()->getTexureObjectID();

	}

	int _solid = 0;
	float _widthTile;
	float _heightTile;
	int _currentAnimateTile = 0;
	int _currentLatency = 0;
	std::vector<int> _tilesOrder;
	std::vector<int> _tilesLatency;

	//
	Buffer _buffer;
	std::shared_ptr<Texture> _texture;
	//
	GLuint _program;
	GLuint _textureObject;
	GLint _aTextureCoordinatesLocation;
	GLint _uTextureUnitLocation;
	GLint _uAdjustXLocation;
	GLint _uAdjustYLocation;
	GLint _uSolidLocation;
	//
	std::string _uSolidString = "u_Solid";
	std::string _aTextureCoordinatesString = "a_TextureCoordinates";
	std::string _uTextureUnitString = "u_TextureUnit";
	std::string _uAdjustX = "u_AdjustX";
	std::string _uAdjustY = "u_AdjustY";
};

class TextComponent : public Component {
public:
	TextComponent() {
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

		FT_Face face;
		if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Set_Pixel_Sizes(face, 0, 48);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (GLubyte c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// Generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Now store character for later use
			Character character = {
				texture,
				std::pair<int, int>(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				std::pair<int, int>(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			_characters.insert(std::pair<GLchar, Character>(c, character));
		}
	}
	
	void setText(std::string text) {
		_text = text;
	}

	std::string _text;

	struct Character {
		GLuint textureID;  // ID handle of the glyph texture
		std::pair<int, int> size;       // Size of glyph
		std::pair<int, int> bearing;    // Offset from baseline to left/top of glyph
		GLuint advance;    // Offset to advance to next glyph
	};

	std::map<GLchar, Character> _characters;
};


enum MoveTypes {
	PlayerControlled = 0,
	StaticallyDefined = 1
};

class MoveComponent : public Component, IMouseEvent {
public:
	void initialize(MoveTypes type, GLuint program, float speed) {
		_type = type;
		_program = program;
		_speed = speed;
		_uMatrixLocation = glGetUniformLocation(_program, _uMatrix.c_str());
		_leftClick = { 0, 0 };
		_rightClick = { 0, 0 };
		_coords = { 0, 0 };
		_move = false;
		if (_type == PlayerControlled)
			MouseEvent::instance().registerComponent(this);
	}

	void initialize(MoveTypes type, GLuint program, int speed, std::tuple<float, float> endPoint) {
		this->initialize(type, program, speed);
		_leftClick = endPoint;
	}

	void mouseClickDownLeft(int x, int y) {
		_leftClick = { x, y };
	}
	void mouseClickDownRight(int x, int y) {
		_rightClick = { x, y };
	}

	void setTransform(std::tuple<float, float> coords) {
		_coords = coords;
	}

	~MoveComponent() {
		if (_type == PlayerControlled)
			MouseEvent::instance().unregisterComponent(this);
	}

	float _speed;
	MoveTypes _type;
	bool _move = false;
	std::tuple<float, float> _leftClick;
	std::tuple<float, float> _rightClick;
	std::tuple<float, float> _coords;
	GLuint _program;
	//
	GLint _uMatrixLocation;
	//
	std::string _uMatrix = "u_Matrix";
};