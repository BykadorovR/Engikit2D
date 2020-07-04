#pragma once
#include "View.h"

class Grid : public View {
private:
	std::tuple<float, float> _dim;
	std::tuple<std::vector<float>, float> _size;
	std::tuple<float, float> _position;
public:
	Grid(std::string name);
	bool initialize();
	bool setPosition(std::tuple<float, float> position);
	bool setDim(std::tuple<float, float> dim);
	std::tuple<float, float> getDim();
	bool setSize(std::tuple<float, float> size);
	bool setSize(std::tuple<std::vector<float>, float> size);
	std::tuple<float, float> getSize();
	std::tuple<float, float> getPosition();
};