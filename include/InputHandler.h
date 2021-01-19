#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <Arduino.h>

#include "Calibrator.h"
#include "GlobalConstants.h"

class InputHandler {
	Calibrator *calibrator = nullptr;

	String getSerialInput();

public:
	InputHandler(Calibrator *calibrator);
	void poll(double thermResistance);
};

#endif
