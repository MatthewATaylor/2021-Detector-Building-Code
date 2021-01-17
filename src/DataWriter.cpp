#include "DataWriter.h"

void DataWriter::setDisplayIsOn(bool displayIsOn) {
	this->displayIsOn = displayIsOn;
}

void DataWriter::setDebugIsOn(bool debugIsOn) {
	this->debugIsOn = debugIsOn;
}

void DataWriter::write(
	int analogValue, double fixedResistorVoltage, double thermVoltage,
	double thermResistance, TempAccumulator *accumulator
) {
	if (!displayIsOn) {
		return;
	}

	if (debugIsOn) {
		Serial.println(
			"Analog Value: " + String(analogValue) +
			" | Resistor V (volts): " + String(fixedResistorVoltage, 4) +
			" | Thermistor V (volts): " + String(thermVoltage, 4) +
			" | Thermistor R (ohms): " + String(thermResistance, 4) +
			" | Temperature (deg. C): " + accumulator->getAvgTempStr(4)
		);
	}
	else {
		Serial.println(
			"Resistor V (volts): " + String(fixedResistorVoltage, 4) +
			" | Thermistor V (volts): " + String(thermVoltage, 4) +
			" | Temperature (deg. C): " + accumulator->getAvgTempStr(4)
		);
	}
}
