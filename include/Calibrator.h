#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <Arduino.h>

#include "GlobalConstants.h"
#include "Mat.h"

class Calibrator {
private:
	static const uint8_t NUM_COEFFICIENTS = 5;

	// Regression coefficients
	const double COEFFICIENTS[NUM_COEFFICIENTS] = {
		0.0011391863,
		0.00022560079,
		0.00000010786971,
		0.0,
		0.0
	};

	// New coefficients set during program execution
	double newCoefficients[NUM_COEFFICIENTS] = {
		0.0,
		0.0,
		0.0,
		0.0,
		0.0
	};

	bool useSteinhartHart = false;
	bool useNewCoefficients = false;

	// Calibration data (temperature recorded in deg. C)
	double resistances[NUM_COEFFICIENTS];
	double temperatures_C[NUM_COEFFICIENTS];
	bool dataRecorded[NUM_COEFFICIENTS] = { 0 };

	// Recursively find number of calibration points, then perform regression
	template <uint8_t MAX_CALIBRATION_POINTS>
	void performRegression(
		double recordedResistances[NUM_COEFFICIENTS],
		double recordedTemperatures_C[NUM_COEFFICIENTS],
		uint8_t numCalibrationPoints
	);

	void updateCoefficients();

public:
	void setData(double resistance, double temperature_C, int dataIndex);
	double getTemperature(double thermVoltage);  // Get temperature in deg. C
	void displayCoefficients();
	void setUseSteinhartHart(bool useSteinhartHart);
	void setUseNewCoefficients(bool useNewCoefficients);
};

#include "../src/Calibrator.inl"

#endif
