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
	MY_TUPLE = 3,
	MY_BOOL = 4,
	MY_CLICK
};

class TextCallback {
public:
	void setValue(float* valueF);
	void setValue(int* valueI);
	void setValue(bool* valueB);
	void setValue(std::string* valueS);
	void setValue(std::vector<float>* valueT);
	void setValue(std::tuple<float, float > * valueC);
	void callToSet(std::string value);

	float* _valueF;
	std::vector<std::string*> _valueS;
	int* _valueI;
	std::vector<float>* _valueT;
	bool* _valueB;
	std::tuple<float, float>* _valueC;
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
	int _defaultX = resolution.first - resolution.first / 5;
	int _defaultY = resolution.second / 10;
	int _defaultWidth = resolution.first / 5;
	int _defaultHeight = 30;
	float _defaultSize = 0.3;

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
		_cameraCoefSpeed = 0;
		_scale = 1;

		assert(_buffer.bindVBO(nullptr, sizeof(float) * 4 * 2, GL_STATIC_DRAW) == TW_OK);
		_aPositionLocation = glGetAttribLocation(_program, _aPositionString.c_str());
	}

	void initializeText(int sceneX, int sceneY, int objectWidth, int objectHeight, GLuint program) {
		_program = program;
		_objectWidth = objectWidth;
		_objectHeight = objectHeight;
		_sceneX = sceneX;
		_sceneY = sceneY;
		_cameraCoefSpeed = 0;
		_scale = 1;
	}

	Matrix2D _transform;
	Matrix2D _camera;
	float _cameraCoefSpeed = 0;
	//
	float _scale = 1;
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
	TextureComponent() {
		_componentName = "TextureComponent";
	}
	void initialize(int textureID, GLuint program) {
		_solid = 0;
		_tilesOrder.clear();
		_tilesLatency.clear();
		_currentAnimateTile = 0;
		_currentLatency = 0;
		_texture = TextureManager::instance()->getTexture(textureID);
		_program = program;

		assert(_buffer.bindVBO(nullptr, 4 * 2 * sizeof(float), GL_DYNAMIC_DRAW) == TW_OK);
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
		_tilesOrder.clear();
		_tilesLatency.clear();
		_currentAnimateTile = 0;
		_currentLatency = 0;
		_texture = texture;
		_program = program;
		assert(_buffer.bindVBO(nullptr, 4 * 2 * sizeof(float), GL_DYNAMIC_DRAW) == TW_OK);
		
		_aTextureCoordinatesLocation = glGetAttribLocation(_program, _aTextureCoordinatesString.c_str());
		_uTextureUnitLocation = glGetUniformLocation(_program, _uTextureUnitString.c_str());
		_uAdjustXLocation = glGetUniformLocation(_program, _uAdjustX.c_str());
		_uAdjustYLocation = glGetUniformLocation(_program, _uAdjustY.c_str());
		_uSolidLocation = glGetUniformLocation(_program, _uSolidString.c_str());
		_textureObject = texture->getAtlas()->getTexureObjectID();

	}

	void initialize(GLuint program) {
		_solid = 1;
		_tilesOrder.clear();
		_tilesLatency.clear();
		_currentAnimateTile = 0;
		_currentLatency = 0;
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
	std::vector<float> _tilesOrder;
	std::vector<float> _tilesLatency;

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
	TextComponent() {
		_componentName = "TextComponent";
	}

	void initialize() {
		_scale = TextHelper::instance()->_defaultSize;
		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);
		glBindVertexArray(_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		KeyboardEvent::instance().registerComponent(this);
	}

	void initialize(std::shared_ptr<TextLoader> loader, std::string text, GLfloat scale, std::vector<float> color, TextType type) {
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
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
	TextType _type = LABEL;
	bool _focus;
	std::string _text = "Empty";
	std::string _textBack = "Empty";
	GLfloat _scale;
	std::vector<float> _color = {1, 0, 0};
	GLuint _VAO, _VBO;
	std::shared_ptr<TextLoader> _loader;
};

enum MoveTypes {
	PlayerControlled = 0,
	StaticallyDefined = 1
};

class MoveComponent : public Component, IMouseEvent, IKeyboardEvent {
public:
	MoveComponent() {
		_componentName = "MoveComponent";
	}
	void initialize(MoveTypes type, GLuint program) {
		_type = type;
		_program = program;
		_uMatrixLocation = glGetUniformLocation(_program, _uMatrix.c_str());
		_leftClick = { 0, 0 };
		_rightClick = { 0, 0 };
		_coords = { 0, 0 };
		_move = false;
		if (_type == PlayerControlled) {
			MouseEvent::instance().registerComponent(this);
			KeyboardEvent::instance().registerComponent(this);
		}
		else {
			MouseEvent::instance().unregisterComponent(this);
			KeyboardEvent::instance().unregisterComponent(this);
		}
	}

	void initialize(MoveTypes type, GLuint program, std::tuple<float, float> endPoint) {
		this->initialize(type, program);
		_leftClick = endPoint;
	}

	void mouseClickDownLeft(int x, int y) {
		if (_speed)
			_leftClick = { x, y };
	}
	void mouseClickDownRight(int x, int y) {
		if (_speed)
			_rightClick = { x, y };
	}

	void keyboardPressed(int character, int action, int mode) {
		if (action == 1) {
			switch (character) {
				case GLFW_KEY_LEFT:
					_direction.first = -1;
				break;
				case GLFW_KEY_RIGHT:
					_direction.first = 1;
				break;
				case GLFW_KEY_UP:
					_direction.second = -1;
				break;
				case GLFW_KEY_DOWN:
					_direction.second = 1;
				break;
			}
		}
		if (action == 0) {
			switch (character) {
			case GLFW_KEY_LEFT:
				_direction.first = 0;
				break;
			case GLFW_KEY_RIGHT:
				_direction.first = 0;
				break;
			case GLFW_KEY_UP:
				_direction.second = 0;
				break;
			case GLFW_KEY_DOWN:
				_direction.second = 0;
				break;
			}
		}
	}

	void setTransform(std::tuple<float, float> coords) {
		_coords = coords;
	}

	~MoveComponent() {
		if (_type == PlayerControlled) {
			MouseEvent::instance().unregisterComponent(this);
			KeyboardEvent::instance().unregisterComponent(this);
		}
	}

	float _speed = 0;
	MoveTypes _type;
	bool _move = false;
	std::tuple<float, float> _leftClick;
	std::tuple<float, float> _rightClick;
	std::tuple<float, float> _coords;
	std::pair<int, int> _direction;
	GLuint _program;
	//
	GLint _uMatrixLocation;
	//
	std::string _uMatrix = "u_Matrix";
};