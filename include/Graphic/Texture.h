#pragma once
#include "platformGL.h"
#include <vector>
#include <string>


using namespace std;

class Texture {
public:
	struct ParentTexture {
		//int because they're coords in atlas
		int _x, _y;
		//use simple pointer to avoid double deallocation (we pass *this to shared_ptr several times but these shared_ptrs 
		//know nothing about themselfs, so they can delete *this several times). We shouldn't delete parent anyway, because it can be used/we can't
		//manage the object we don't know anything about
		Texture* _atlas;

		/*Texture, x, y*/
		ParentTexture(Texture* atlas, std::pair<int, int> coords) : _atlas(atlas), _x(coords.first), _y(coords.second) {
		}
	};

	Texture(int width, int height);
	Texture(string path);
	Texture();

	GLuint loadTexture();
	void addTexture(Texture& image, int startX, int startY);

	int getWidth() {
		return _width;
	}

	int getHeight() {
		return _height;
	}

	unsigned char* getData() {
		return &_data[0];
	}

	unsigned char& operator[](int index) {
		return _data[index];
	}

	std::shared_ptr<ParentTexture> getParent() {
		return _parent;
	}

	void setParent(std::shared_ptr<ParentTexture> parent) {
		_parent = parent;
	}
protected:
	int _width, _height;
	std::vector<unsigned char> _data;
	//it will be initialized only if some other texture attaches this texture to itself
	std::shared_ptr<ParentTexture> _parent = nullptr;
};