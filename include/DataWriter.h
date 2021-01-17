#ifndef DATA_WRITER_H
#define DATA_WRITER_H

#include <Arduino.h>

#include "TempAccumulator.h"

class DataWriter {
private:
	bool displayIsOn = true;
	bool debugIsOn = true;

public:
	void setDisplayIsOn(bool displayIsOn);
	void setDebugIsOn(bool debugIsOn);
	void write(
		int analogValue, double fixedResistorVoltage, double thermVoltage,
		double thermResistance, TempAccumulator *accumulator
	);
};

#endif
