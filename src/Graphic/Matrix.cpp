#include "Matrix.h"
#include "math.h"
#include "Common.h"

Matrix2D::Matrix2D() {
	_matrix.resize(_size * _size);
	identity();
}

void Matrix2D::print() {
	for (int i = 0; i < _size; i++) {
		for (int j = 0; j < _size; j++) {
			printf("%f ", _matrix[j + i * _size]);
		}
		printf("\n");
	}
}

void Matrix2D::identity() {
	for (int i = 0; i < _size; i++) 
		for (int j = 0; j < _size; j++) {
			_matrix[j + _size * i] = 0;
			if (i == j)
				_matrix[j + _size * i] = 1;
		}
}
void Matrix2D::translate(float dx, float dy) {
	if (dx) {
		float dxN = (float)(dx) / (float)resolution.first;
		_matrix[3 + _size * 0] = dxN;
	}
	if (dy) {
		float dyN = (float)(dy) / (float)resolution.second;
		_matrix[3 + _size * 1] =  - dyN;
	}
}
void Matrix2D::rotate(float angle) {
	_matrix[0 + _size * 0] = cos(angle);
	_matrix[1 + _size * 0] = sin(angle);
	_matrix[0 + _size * 1] = -sin(angle);
	_matrix[1 + _size * 1] = cos(angle);
}
void Matrix2D::scale(int scaleX, int scaleY) {
	_matrix[0 + _size * 0] = scaleX;
	_matrix[1 + _size * 1] = scaleY;
}

float* Matrix2D::getData() {
	return _matrix.data();
}

void Matrix2D::operator=(Matrix2D& right) {
	for (int i = 0; i < _size; i++)
		for (int j = 0; j < _size; j++) {
			_matrix[j + i * _size] = right[j + i * _size];
		}
}

float& Matrix2D::operator[](int index) {
	return _matrix[index];
}

Matrix2D Matrix2D::operator*(Matrix2D &right) {
	Matrix2D temp;
	for (int j = 0; j < _size; j++) {
		for (int k = 0; k < _size; k++) {
			float sum = 0;
			for (int i = 0; i < _size; i++) {
				sum += _matrix[i + j * _size] * right[k + i * _size];
			}
			temp[k + j * _size] = sum;
		}
	}
	return temp;
}