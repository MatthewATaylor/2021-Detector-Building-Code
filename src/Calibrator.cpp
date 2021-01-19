#include "Calibrator.h"

void Calibrator::updateCoefficients() {
	// Use three calibration points to calculate A, B, and C coefficients
	double lnR1 = log(resistances[0]);
	double lnR2 = log(resistances[1]);
	double lnR3 = log(resistances[2]);

	double invT1 = 1 / temperatures_K[0];
	double invT2 = 1 / temperatures_K[1];
	double invT3 = 1 / temperatures_K[2];

	double delta2 = (invT2 - invT1) / (lnR2 - lnR1);
	double delta3 = (invT3 - invT1) / (lnR3 - lnR1);

	double C = (delta3 - delta2) / ((lnR3 - lnR2) * (lnR1 + lnR2 + lnR3));
	double B = delta2 - C * (lnR1 * lnR1 + lnR1 * lnR2 + lnR2 * lnR2);
	double A = invT1 - lnR1 * (B + C * lnR1 * lnR1);

	coefficients.newA = A;
	coefficients.newB = B;
	coefficients.newC = C;
}

void Calibrator::setData(double temperature_K, double resistance, int dataIndex) {
	if (dataIndex >= 0 && dataIndex <= 2) {
		temperatures_K[dataIndex] = temperature_K;
		resistances[dataIndex] = resistance;
		dataRecorded[dataIndex] = true;

		// Three data points collected, display coefficients
		if (dataRecorded[0] && dataRecorded[1] && dataRecorded[2]) {
			displayCoefficients();
		}
	}
}

double Calibrator::getTemperature(double thermVoltage) {
	double fixedResistorVoltage = GlobalConstants::V_OUT - thermVoltage;
	double current = fixedResistorVoltage / GlobalConstants::FIXED_RESISTANCE;
	double thermResistance = thermVoltage / current;

	return (
		1.0 / (
			Coefficients::A +
			Coefficients::B * log(thermResistance) +
			Coefficients::C * pow(log(thermResistance), 3)
		)
	) - GlobalConstants::KELVIN_CONVERSION;
}

void Calibrator::displayCoefficients() {
	updateCoefficients();
	Serial.println(
		"COEFFICIENTS | A = " + String(coefficients.newA, 16) +
		" | B = " + String(coefficients.newB, 16) +
		" | C = " + String(coefficients.newC, 16)
	);
}
