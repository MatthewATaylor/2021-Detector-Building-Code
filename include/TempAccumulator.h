#ifndef TEMP_ACCUMULATOR_H
#define TEMP_ACCUMULATOR_H

#include <Arduino.h>

class TempAccumulator {
private:
	static const int NUM_TEMPERATURES = 15;
	static const int NUM_AVERAGED_TEMPERATURES = 9;

	double prevTemperatures[NUM_TEMPERATURES] = { 0 };
	int newTemperatureIndex = 0;
	int canDisplayCounter = 0;
	bool canDisplayTemp = false;

	// For sorting temperatures
	static int compare(const void *num1, const void *num2);

public:
	double getAvgTemp();
	String getAvgTempStr(int decimalPlaces);
	void addTemperature(double temperature);
};

#endif
