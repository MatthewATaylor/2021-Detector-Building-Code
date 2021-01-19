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

	// Calibration data (temperature recorded in K)
	double temperatures_K[3];
	double resistances[3];
	bool dataRecorded[3] = { 0 };

	void updateCoefficients();

public:
	void setData(double temperature_K, double resistance, int dataIndex);
	double getTemperature(double thermVoltage);
	void displayCoefficients();
};

#endif
