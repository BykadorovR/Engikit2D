#include "Texture.h"
#include "Object.h"

class Sprite : public Object {
public:
	Sprite(int sceneX, int sceneY, int spriteWidth, int spriteHeight, Texture texture);
};

class AnimatedSprite : public Object {
public:
	AnimatedSprite(int sceneX, int sceneY, int spriteWidth, int spriteHeight, Texture texture);
	void setAnimate(std::vector<int> tiles, std::vector<int> latency);
	int getCurrentAnimateTile();
	void draw() override;
	void attach() override;
protected:
	void setBuffer() override;
	float _widthTile;
	float _heightTile;
	int _currentAnimateTile = 0;
	int _currentLatency = 0;
	std::vector<int> _tilesOrder;
	std::vector<int> _tilesLatency;
	GLint _uAdjustXLocation;
	GLint _uAdjustYLocation;
	std::string _uAdjustX = "u_AdjustX";
	std::string _uAdjustY = "u_AdjustY";
};