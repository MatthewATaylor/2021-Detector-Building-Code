#ifndef MAT_H
#define MAT_H

#include <Arduino.h>

template<typename T, size_t ROWS, size_t COLS>
class Mat {
private:
	template<typename LambdaType>
	void forAllElements(LambdaType lambda) const;

	template<typename LambdaType>
	void forAllElements(LambdaType lambda);

public:
	T elements[ROWS][COLS];

	static Mat<T, ROWS, COLS> identity();

	Mat();
	Mat(T elements[ROWS][COLS]);

	Mat<T, ROWS, COLS> &operator=(const Mat<T, ROWS, COLS> &otherMat);
	Mat<T, ROWS, COLS> &operator+=(const Mat<T, ROWS, COLS> &otherMat);
	Mat<T, ROWS, COLS> &operator-=(const Mat<T, ROWS, COLS> &otherMat);

	Mat<T, ROWS, COLS> operator+(const Mat<T, ROWS, COLS> &otherMat) const;
	Mat<T, ROWS, COLS> operator-(const Mat<T, ROWS, COLS> &otherMat) const;
	template<size_t OTHER_COLS>
	Mat<T, ROWS, OTHER_COLS> operator*(const Mat<T, COLS, OTHER_COLS> &otherMat) const;

	bool operator==(const Mat<T, ROWS, COLS> &otherMat) const;

	T &operator()(size_t row, size_t col);
	const T &operator()(size_t row, size_t col) const;

	const T *getPtr() const;

	void print(int decimalPlaces = 4) const;

	Mat<T, COLS, ROWS> transpose() const;
	Mat<T, ROWS, COLS> inverse() const;
};

typedef Mat<double, 2, 2> Mat2;
typedef Mat<double, 3, 3> Mat3;
typedef Mat<double, 4, 4> Mat4;
typedef Mat<double, 5, 5> Mat5;

#include "../src/Mat.inl"

#endif
