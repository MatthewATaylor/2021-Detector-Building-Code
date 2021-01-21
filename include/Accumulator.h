#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include <Arduino.h>

template<typename T>
class Accumulator {
private:
	static const int NUM_VALUES = 15;
	static const int NUM_AVERAGED_VALUES = 9;

	T prevValues[NUM_VALUES] = { 0 };
	int newValueIndex = 0;
	int enoughDataCounter = 0;

	// For sorting values
	static int compare(const void *num1, const void *num2);

public:
	bool hasEnoughData = false;

	double getAvgValue();
	void addValue(T value);
};

#include "../src/Accumulator.inl"

#endif
