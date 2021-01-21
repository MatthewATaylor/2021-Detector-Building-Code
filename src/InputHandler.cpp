#include "InputHandler.h"

String InputHandler::getSerialInput() {
	String stringInput = "";
	if (Serial.available() > 0) {
		// Read serial input
		stringInput = Serial.readString();
		stringInput.trim();
		stringInput.toLowerCase();
		Serial.println("INPUT | " + stringInput);
	}
	return stringInput;
}

InputHandler::InputHandler(Calibrator *calibrator) : calibrator(calibrator) {}

void InputHandler::poll(double thermResistance, double thermVoltage) {
	// Read from serial monitor
	String command = getSerialInput();

	if (command.length() == 0) {
		// No input data
		return;
	}

	// Set calibration point temperature (example command: set 1 25.3)
	if (command.length() > 6 && command.startsWith("set ") &&
		command[4] >= '0' && command[4] <= '4'
	) {
		String temperatureString = command.substring(6);
		double temperature_C = temperatureString.toDouble();
		int calibrationDataIndex = command[4] - '0';
		calibrator->setData(thermResistance, temperature_C, calibrationDataIndex);

		// Print resistance for verification
		Serial.println(
			"CALIBRATION | " + String(command[3]) +
			" | V = " + String(thermVoltage, 4)
		);
	}

	else if (command == "get coefficients") {
		calibrator->displayCoefficients();
	}

	// Use Steinhart-Hart equation? (false by default)
	else if (command == "set sh true") {
		calibrator->setUseSteinhartHart(true);
	}
	else if (command == "set sh false") {
		calibrator->setUseSteinhartHart(false);
	}

	// Use new coefficients? (false by default)
	else if (command == "set newc true") {
		calibrator->setUseNewCoefficients(true);
	}
	else if (command == "set newc false") {
		calibrator->setUseNewCoefficients(false);
	}
}
