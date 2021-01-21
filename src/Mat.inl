template<typename T, size_t ROWS, size_t COLS>
template<typename LambdaType>
inline void Mat<T, ROWS, COLS>::forAllElements(LambdaType lambda) const {
	for (size_t i = 0; i < ROWS; ++i) {
		for (size_t j = 0; j < COLS; ++j) {
			lambda(i, j);
		}
	}
}
template<typename T, size_t ROWS, size_t COLS>
template<typename LambdaType>
inline void Mat<T, ROWS, COLS>::forAllElements(LambdaType lambda) {
	for (size_t i = 0; i < ROWS; ++i) {
		for (size_t j = 0; j < COLS; ++j) {
			lambda(i, j);
		}
	}
}

template<typename T, size_t ROWS, size_t COLS>
inline Mat<T, ROWS, COLS> Mat<T, ROWS, COLS>::identity() {
	Mat<T, ROWS, COLS> result;
	result.forAllElements([&](size_t row, size_t col) -> void {
		if (row == col) {
			result.elements[row][col] = 1;
		}
	});
	return result;
}

template<typename T, size_t ROWS, size_t COLS>
inline Mat<T, ROWS, COLS>::Mat() {
	forAllElements([&](size_t row, size_t col) -> void {
		elements[row][col] = 0;
	});
}
template<typename T, size_t ROWS, size_t COLS>
inline Mat<T, ROWS, COLS>::Mat(T elements[ROWS][COLS]) {
	forAllElements([&](size_t row, size_t col) -> void {
		this->elements[row][col] = elements[row][col];
	});
}

template<typename T, size_t ROWS, size_t COLS>
inline Mat<T, ROWS, COLS> &Mat<T, ROWS, COLS>::operator=(const Mat<T, ROWS, COLS> &otherMat) {
	forAllElements([&](size_t row, size_t col) -> void {
		elements[row][col] = otherMat.elements[row][col];
	});
	return *this;
}
template<typename T, size_t ROWS, size_t COLS>
inline Mat<T, ROWS, COLS> &Mat<T, ROWS, COLS>::operator+=(const Mat<T, ROWS, COLS> &otherMat) {
	forAllElements([&](size_t row, size_t col) -> void {
		elements[row][col] += otherMat.elements[row][col];
	});
	return *this;
}
template<typename T, size_t ROWS, size_t COLS>
inline Mat<T, ROWS, COLS> &Mat<T, ROWS, COLS>::operator-=(const Mat<T, ROWS, COLS> &otherMat) {
	forAllElements([&](size_t row, size_t col) -> void {
		elements[row][col] -= otherMat.elements[row][col];
	});
	return *this;
}

template<typename T, size_t ROWS, size_t COLS>
inline Mat<T, ROWS, COLS> Mat<T, ROWS, COLS>::operator+(const Mat<T, ROWS, COLS> &otherMat) const {
	Mat<T, ROWS, COLS> newMat = *this;
	return newMat += otherMat;
}
template<typename T, size_t ROWS, size_t COLS>
inline Mat<T, ROWS, COLS> Mat<T, ROWS, COLS>::operator-(const Mat<T, ROWS, COLS> &otherMat) const {
	Mat<T, ROWS, COLS> newMat = *this;
	return newMat -= otherMat;
}
template<typename T, size_t ROWS, size_t COLS>
template<size_t OTHER_COLS>
inline Mat<T, ROWS, OTHER_COLS> Mat<T, ROWS, COLS>::operator*(const Mat<T, COLS, OTHER_COLS> &otherMat) const {
	Mat<T, ROWS, OTHER_COLS> newMat;
	for (size_t i = 0; i < ROWS; ++i) {
		for (size_t j = 0; j < OTHER_COLS; ++j) {
			T newElement = 0;
			for (size_t k = 0; k < COLS; ++k) {
				newElement += elements[i][k] * otherMat.elements[k][j];
			}
			newMat.elements[i][j] = newElement;
		}
	}
	return newMat;
}

template<typename T, size_t ROWS, size_t COLS>
inline bool Mat<T, ROWS, COLS>::operator==(const Mat<T, ROWS, COLS> &otherMat) const {
	for (size_t i = 0; i < ROWS; ++i) {
		for (size_t j = 0; j < COLS; ++j) {
			if (elements[i][j] != otherMat.elements[i][j]) {
				return false;
			}
		}
	}
	return true;
}

template<typename T, size_t ROWS, size_t COLS>
T &Mat<T, ROWS, COLS>::operator()(size_t row, size_t col) {
	return elements[row][col];
}
template<typename T, size_t ROWS, size_t COLS>
const T &Mat<T, ROWS, COLS>::operator()(size_t row, size_t col) const {
	return elements[row][col];
}

template <typename T, size_t ROWS, size_t COLS>
inline const T *Mat<T, ROWS, COLS>::getPtr() const {
	return &(elements[0][0]);
}

template <typename T, size_t ROWS, size_t COLS>
inline void Mat<T, ROWS, COLS>::print(int decimalPlaces) const {
	for (size_t i = 0; i < ROWS; ++i) {
		for (size_t j = 0; j < COLS; ++j) {
			if (i == 0 && j == 0) {
				Serial.print("[");
			}
			else {
				Serial.print(" ");
			}
			Serial.print(String(elements[i][j], decimalPlaces));
			if (i == ROWS - 1 && j == COLS - 1) {
				Serial.print("]");
			}
			else {
				Serial.print(" ");
			}
		}
		Serial.println();
	}
	Serial.println();
}

template<typename T, size_t ROWS, size_t COLS>
Mat<T, COLS, ROWS> Mat<T, ROWS, COLS>::transpose() const {
	T newElements[COLS][ROWS];
	for (size_t i = 0; i < ROWS; ++i) {
		for (size_t j = 0; j < COLS; ++j) {
			newElements[j][i] = elements[i][j];
		}
	}
	Mat<T, COLS, ROWS> newMat(newElements);
	return newMat;
}

template<typename T, size_t ROWS, size_t COLS>
Mat<T, ROWS, COLS> Mat<T, ROWS, COLS>::inverse() const {
	static_assert(ROWS == COLS, "Matrix must be square");
	Mat<T, ROWS, COLS> result = Mat<T, ROWS, COLS>::identity();
	Mat<T, ROWS, COLS> current = *this;
	for (size_t i = 0; i < COLS; ++i) {
		// Divide row to get 1 in desired position
		T divisor = current(i, i);
		for (size_t j = 0; j < COLS; ++j) {
			current(i, j) /= divisor;
			result(i, j) /= divisor;
		}

		// Subtract rows by a number to get 0 in desired positions
		for (size_t j = 0; j < ROWS; ++j) {
			if (j == i) {
				continue;
			}
			T multiplier = current(j, i);
			for (size_t k = 0; k < COLS; ++k) {
				current(j, k) -= multiplier * current(i, k);
				result(j, k) -= multiplier * result(i, k);
			}
		}
	}
	return result;
}
