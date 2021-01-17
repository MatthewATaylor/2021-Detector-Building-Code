#include "InputHandler.h"

const String InputHandler::Commands::DISPLAY_ON = "display on";
const String InputHandler::Commands::DISPLAY_OFF = "display off";
const String InputHandler::Commands::DISPLAY_DEBUG_ON = "display debug on";
const String InputHandler::Commands::DISPLAY_DEBUG_OFF = "display debug off";
const String InputHandler::Commands::YES = "y";
const String InputHandler::Commands::NO = "n";
const String InputHandler::Commands::GET_COEFFICIENTS = "get coefficients";
const String InputHandler::Commands::HELP = "help";

String InputHandler::getSerialInput() {
	String stringInput = "";
	if (Serial.available() > 0) {
		// Read string entered into serial monitor
		stringInput = Serial.readString();
		stringInput.remove(stringInput.length() - 1);
		stringInput.toLowerCase();
	}
	return stringInput;
}

void InputHandler::echoInput(const String &input) {
	// Display commands entered into serial monitor
	Serial.println();
	Serial.println("> " + input);
}

InputHandler::InputHandler(Calibrator *calibrator, DataWriter *dataWriter) :
	calibrator(calibrator), dataWriter(dataWriter) {}

void InputHandler::poll(double thermResistance) {
	// Read from serial monitor
	String command = getSerialInput();
	if (command.length() == 0) {
		return;
	}
	echoInput(command);

	// Respond to command input
	bool isValidInput = true;
	if (isVerifyingCalibration) {
		if (command.equals(Commands::YES) || command.equals(Commands::NO)) {
			bool shouldDisableDisplay = false;
			if (command.equals(Commands::YES)) {
				shouldDisableDisplay = calibrator->verifyTempData();
			}
			dataWriter->setDisplayIsOn(!shouldDisableDisplay);
			isVerifyingCalibration = false;
		}
		else {
			isValidInput = false;
		}
	}
	else {
		if (command.equals(Commands::DISPLAY_ON)) {
			dataWriter->setDisplayIsOn(true);
		}
		else if (command.equals(Commands::DISPLAY_OFF)) {
			dataWriter->setDisplayIsOn(false);
		}
		else if (command.equals(Commands::DISPLAY_DEBUG_ON)) {
			dataWriter->setDebugIsOn(true);
		}
		else if (command.equals(Commands::DISPLAY_DEBUG_OFF)) {
			dataWriter->setDebugIsOn(false);
		}
		else if (command.equals(Commands::GET_COEFFICIENTS)) {
			calibrator->displayCoefficients();
			dataWriter->setDisplayIsOn(false);
		}
		else if (command.equals(Commands::HELP)) {
			Serial.println("command: description");
			Serial.println(Commands::DISPLAY_ON + ": turn on data display");
			Serial.println(Commands::DISPLAY_OFF + ": turn off data display");
			Serial.println(Commands::DISPLAY_DEBUG_ON + ": turn on debug info display");
			Serial.println(Commands::DISPLAY_DEBUG_OFF + ": turn off debug info display");
			Serial.println("set[1-3] [temperature]: set data point for calibration");
			Serial.println(
				Commands::GET_COEFFICIENTS +
				": get updated Steinhart-Hart coefficients following calibration"
			);
		}
		else if (command.length() > 5 && command.startsWith("set") &&
			(command[3] == '1' || command[3] == '2' || command[3] == '3')
		) {
			String temperatureString = command.substring(5);
			double temperature = temperatureString.toDouble();
			double temperature_K = temperature + GlobalConstants::KELVIN_CONVERSION;
			int calibrationDataIndex = command[3] - '0' - 1;
			calibrator->setTempData(temperature_K, thermResistance, calibrationDataIndex);
			Serial.println(
				"Set calibration point " + String(command[3]) +
				" to temperature: " + temperatureString +
				" and resistance: " + String(thermResistance, 4) + "? (y/n)"
			);
			isVerifyingCalibration = true;
		}
		else {
			isValidInput = false;
		}
	}

	if (!isValidInput) {
		Serial.println("Invalid input");
	}
}
