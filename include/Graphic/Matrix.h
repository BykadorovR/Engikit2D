#pragma once
#include "vector"

class Matrix2D {
public:
	Matrix2D();

	void identity();
	void translate(int dx, int dy);
	void rotate(float angle);
	void scale(int scaleX, int scaleY);
	void print();

	float* getData();

	void operator=(Matrix2D& right);
	float& operator[](int index);
	Matrix2D operator*(Matrix2D &right);

private:
	std::vector<float> _matrix;
	int _size = 4;
};