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

void InputHandler::poll(double thermResistance) {
	// Read from serial monitor
	String command = getSerialInput();
	if (command.length() == 0) {
		return;
	}

	// Set calibration point temperature (example command: set1 25.3)
	if (command.length() > 5 && command.startsWith("set") &&
		(command[3] == '0' || command[3] == '1' || command[3] == '2')
	) {
		String temperatureString = command.substring(5);
		double temperature = temperatureString.toDouble();
		double temperature_K = temperature + GlobalConstants::KELVIN_CONVERSION;
		int calibrationDataIndex = command[3] - '0';
		calibrator->setData(temperature_K, thermResistance, calibrationDataIndex);

		// Print resistance for verification
		Serial.println(
			"CALIBRATION | " + String(command[3]) +
			" | R = " + String(thermResistance, 2)
		);
	}
}
