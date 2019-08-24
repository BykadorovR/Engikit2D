#pragma once
#include "Component.h"
#include "Common.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Matrix.h"
#include "Events.h"
#include "TextLoader.h"
#include "TextHelper.h"

enum TextConversion {
	MY_FLOAT = 0,
	MY_INT = 1,
	MY_STRING = 2,
	MY_TUPLE
};

class TextCallback {
public:
	void setValue(float* valueF);
	void setValue(int* valueI);
	void setValue(std::string* valueS);
	void setValue(int** valueT);
	void callToSet(std::string value);

	float* _valueF;
	std::string* _valueS;
	int* _valueI;
	int** _valueT;
	TextConversion _conversion;
};

class TextHelper {
public:
	static TextHelper* instance();
	std::shared_ptr<Entity> createText(std::string text, int x, int y, int width, int height, float scale, bool edit = false);
	void attachText(std::shared_ptr<Entity> entity);
	bool detachText(std::shared_ptr<Entity> entity);
	void getValue(std::shared_ptr<TextCallback> callback, std::string text, int x, int y, int width, int height, float scale);
	std::vector<std::shared_ptr<Entity> > _buffer;
private:
	TextHelper();

	int _bufferSize = 5;

};


class ObjectComponent : public Component {
public:
	ObjectComponent() {
		_componentName = "ObjectComponent";
	}
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

	void initializeText(int sceneX, int sceneY, int objectWidth, int objectHeight, GLuint program) {
		_program = program;
		_objectWidth = objectWidth;
		_objectHeight = objectHeight;
		_sceneX = sceneX;
		_sceneY = sceneY;
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
		_componentName = "TextureComponent";
		_solid = 0;
		_texture = TextureManager::instance()->getTexture(textureID);
		_program = program;
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
	void initialize(std::shared_ptr<Texture> texture, GLuint program) {
		_componentName = "TextureComponent";
		_solid = 0;
		_texture = texture;
		_program = program;
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

	void initialize(GLuint program) {
		_solid = 1;
		_texture = nullptr;
		_program = program;
		_uSolidLocation = glGetUniformLocation(_program, _uSolidString.c_str());
	}

	int _solid = 0;
	int _textureID = -1;
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

enum TextType {
	LABEL = 0,
	EDIT = 1
};

class TextComponent : public Component, IKeyboardEvent {
public:
	void initialize(std::shared_ptr<TextLoader> loader, std::string text, GLfloat scale, std::vector<float> color, TextType type) {
		_componentName = "TextComponent";
		_loader = loader;
		_text = text;
		_textBack = text;
		_scale = scale;
		_color = color;
		_type = type;
		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);
		glBindVertexArray(_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		KeyboardEvent::instance().registerComponent(this);
	}
	
	void keyboardPressed(int character, int action, int mode) {
		if (!_focus)
			return;
		if (action == 1) {
			if (character == GLFW_KEY_BACKSPACE) {
				_text = _text.substr(0, _text.size() - 1);
			}
			else {
				unsigned char c = (unsigned char)character;
				if (mode == GLFW_MOD_SHIFT) {
					if (character != GLFW_KEY_LEFT_SHIFT) {
						if (islower(c))
							c = toupper(c);
						_text += c;
					}
				}
				else {
					if (isupper(c)) 
						c = tolower(c);
					_text += c;
				}
				
			}

		}
	}

	~TextComponent() {
		KeyboardEvent::instance().unregisterComponent(this);
	}

	void setCallback(std::shared_ptr<TextCallback> callback) {
		_callback = callback;
	}

	void setFocus(bool focus) {
		if (focus == false && _focus == true) {
			if (_callback)
				_callback->callToSet(_text);
			//need to restore label back
			_text = _textBack;
		}
		_focus = focus;
	}

	std::shared_ptr<TextCallback> _callback;
	TextType _type;
	bool _focus;
	std::string _text;
	std::string _textBack;
	GLfloat _scale;
	std::vector<float> _color;
	GLuint _VAO, _VBO;
	GLuint _program;
	std::shared_ptr<TextLoader> _loader;
};

enum MoveTypes {
	PlayerControlled = 0,
	StaticallyDefined = 1
};

class MoveComponent : public Component, IMouseEvent {
public:
	void initialize(MoveTypes type, GLuint program, float speed) {
		_componentName = "MoveComponent";
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
		_componentName = "MoveComponent";
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