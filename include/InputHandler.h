#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <Arduino.h>

#include "Calibrator.h"
#include "DataWriter.h"
#include "GlobalConstants.h"

class InputHandler {
	struct Commands {
		const static String DISPLAY_ON;
		const static String DISPLAY_OFF;
		const static String DISPLAY_DEBUG_ON;
		const static String DISPLAY_DEBUG_OFF;
		const static String YES;
		const static String NO;
		const static String GET_COEFFICIENTS;
		const static String HELP;
	};

	Calibrator *calibrator = nullptr;
	DataWriter *dataWriter = nullptr;

	bool isVerifyingCalibration = false;

	String getSerialInput();
	void echoInput(const String &input);

public:
	InputHandler(Calibrator *calibrator, DataWriter *dataWriter);
	void poll(double thermResistance);
};

#endif
