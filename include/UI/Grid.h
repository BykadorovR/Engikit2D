#pragma once
#include "View.h"

class Grid : public View {
private:
	std::tuple<float, float> _dim;
	std::vector<std::tuple<float, float> > _size;
	std::tuple<float, float> _position;
public:
	Grid(std::string name);
	bool initialize();
	bool setPosition(std::tuple<float, float> position);
	bool setDim(std::tuple<float, float> dim);
	bool setSize(std::vector<std::tuple<float, float> > size);
	std::tuple<float, float> getSize();

};