#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <Arduino.h>

#include "GlobalConstants.h"

class Calibrator {
private:
	// Steinhart-Hart coefficients
	struct Coefficients {
		static constexpr double A = 0.00063017407;
		static constexpr double B = 0.00023429448;
		static constexpr double C = 0.000000030105159;

		double newA = A;
		double newB = B;
		double newC = C;
	} coefficients;

	// Calibration data to be verified
	int newDataIndex = -1;
	double tempTemperature_K;
	double tempResistance;

	// Calibration data (temperature recorded in K)
	double temperatures_K[3];
	double resistances[3];
	bool dataRecorded[3] = { 0 };

	void updateCoefficients();

public:
	void setTempData(double temperature_K, double resistance, int dataIndex);
	bool verifyTempData();  // Returns true if all needed data has been collected
	double getTemperature(double thermVoltage);
	void displayCoefficients();
};

#endif
