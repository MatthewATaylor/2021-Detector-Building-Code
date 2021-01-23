template <uint8_t MAX_CALIBRATION_POINTS>
inline void Calibrator::performRegression(
	double recordedResistances[NUM_COEFFICIENTS],
	double recordedTemperatures_C[NUM_COEFFICIENTS],
	uint8_t numCalibrationPoints
) {
	if (MAX_CALIBRATION_POINTS == numCalibrationPoints) {
		// Calibration point numbers match, solve for coefficients using matrices
		// [resistance matrix][coefficient matrix] = [temperature matrix]
		// [coefficient matrix] = [resistance matrix]^-1[temperature matrix]

		Mat<double, MAX_CALIBRATION_POINTS, MAX_CALIBRATION_POINTS> resistanceMat;
		for (uint8_t i = 0; i < numCalibrationPoints; ++i) {
			for (uint8_t j = 0; j < numCalibrationPoints; ++j) {
				if (useSteinhartHart) {
					uint8_t exponent = j < 2 ? j : j + 1;  // Squared term found to reduce accuracy
					resistanceMat(i, j) = pow(log(recordedResistances[i]), exponent);
				}
				else {
					resistanceMat(i, j) = pow(recordedResistances[i], j);
				}
			}
		}

		Mat<double, MAX_CALIBRATION_POINTS, 1> temperatureMat;
		for (uint8_t i = 0; i < numCalibrationPoints; ++i) {
			if (useSteinhartHart) {
				temperatureMat(i, 0) =
					1.0 / (recordedTemperatures_C[i] + GlobalConstants::KELVIN_CONVERSION);
			}
			else {
				temperatureMat(i, 0) = recordedTemperatures_C[i];
			}
		}

		Mat<double, MAX_CALIBRATION_POINTS, 1> coefficientMat = resistanceMat.inverse() * temperatureMat;
		for (uint8_t i = 0; i < numCalibrationPoints; ++i) {
			newCoefficients[i] = coefficientMat(i, 0);
		}
	}
	else {
		// Try a smaller calibration point number
		performRegression<MAX_CALIBRATION_POINTS - 1>(
			recordedResistances, recordedTemperatures_C, numCalibrationPoints
		);
	}
}

// End recursion at MAX_CALIBRATION_POINTS == 1
template <>
inline void Calibrator::performRegression<1>(
	double recordedResistances[NUM_COEFFICIENTS],
	double recordedTemperatures_C[NUM_COEFFICIENTS],
	uint8_t numCalibrationPoints
) {}

inline void Calibrator::updateCoefficients() {
	// Gather recorded calibration data
	double recordedResistances[NUM_COEFFICIENTS];
	double recordedTemperatures_C[NUM_COEFFICIENTS];
	uint8_t numCalibrationPoints = 0;
	for (uint8_t i = 0; i < NUM_COEFFICIENTS; ++i) {
		if (dataRecorded[i]) {
			recordedResistances[numCalibrationPoints] = resistances[i];
			recordedTemperatures_C[numCalibrationPoints] = temperatures_C[i];
			++numCalibrationPoints;
		}
	}

	// Use function template recursion for regression because Mat requires constexpr dimensions
	performRegression<NUM_COEFFICIENTS>(
		recordedResistances, recordedTemperatures_C, numCalibrationPoints
	);
}

inline void Calibrator::setData(double resistance, double temperature_C, int dataIndex) {
	if (dataIndex >= 0 && dataIndex <= 4) {
		temperatures_C[dataIndex] = temperature_C;
		resistances[dataIndex] = resistance;
		dataRecorded[dataIndex] = true;

		uint8_t numPointsRecorded = 0;
		for (uint8_t i = 0; i < NUM_COEFFICIENTS; ++i) {
			numPointsRecorded += dataRecorded[i];
		}

		// Show coefficients after receiving enough calibration points
		if (numPointsRecorded >= 2) {
			displayCoefficients();
		}
	}
}

inline double Calibrator::getTemperature(double thermVoltage) {
	double fixedResistorVoltage = GlobalConstants::V_OUT - thermVoltage;
	double current = fixedResistorVoltage / GlobalConstants::FIXED_RESISTANCE;
	double thermResistance = thermVoltage / current;

	double coefficients[NUM_COEFFICIENTS];
	for (uint8_t i = 0; i < NUM_COEFFICIENTS; ++i) {
		coefficients[i] = useNewCoefficients ? newCoefficients[i] : COEFFICIENTS[i];
	}

	double temperature = 0.0;
	if (useSteinhartHart) {
		// Steinhart-Hart model
		for (uint8_t i = 0; i < NUM_COEFFICIENTS; ++i) {
			uint8_t exponent = i < 2 ? i : i + 1;  // Squared term found to reduce accuracy
			temperature += coefficients[i] * pow(log(thermResistance), exponent);
		}
		temperature = 1.0 / temperature;
		temperature -= GlobalConstants::KELVIN_CONVERSION;
	}
	else {
		// Polynomial model
		for (uint8_t i = 0; i < NUM_COEFFICIENTS; ++i) {
			temperature += coefficients[i] * pow(thermResistance, i);
		}
	}

	return temperature;
}

inline void Calibrator::displayCoefficients() {
	updateCoefficients();
	Serial.print("COEFFICIENTS");
	for (uint8_t i = 0; i < NUM_COEFFICIENTS; ++i) {
		Serial.print(" | " + String(newCoefficients[i], 16));
	}
	Serial.println();
}

inline void Calibrator::setUseSteinhartHart(bool useSteinhartHart) {
	this->useSteinhartHart = useSteinhartHart;
	displayCoefficients();
}

inline void Calibrator::setUseNewCoefficients(bool useNewCoefficients) {
	this->useNewCoefficients = useNewCoefficients;
	displayCoefficients();
}
