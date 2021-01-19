#include <Arduino.h>
#include <Adafruit_ADS1015.h>

#include "Accumulator.h"
#include "Calibrator.h"
#include "InputHandler.h"
#include "GlobalConstants.h"

const double ADC_MAX_VALUE = 32767.0;  // 15-bit ADC
const double ADC_MAX_VOLTAGE = 4.096;  // With gain of 1

// Digital and analog IO pin numbers
struct Pins {
	const static int THERMISTOR = 0;
	const static int RED = 2;
	const static int GREEN = 3;
	const static int BLUE = 4;
};

Adafruit_ADS1115 adc;
Accumulator<int> analogAccumulator;
Calibrator calibrator;
InputHandler inputHandler(&calibrator);

void controlLEDs(double temperature) {
	// Control blue LED
	if (temperature >= 0 && temperature <= 20) {
		digitalWrite(Pins::BLUE, HIGH);
	}
	else {
		digitalWrite(Pins::BLUE, LOW);
	}

	// Control green LED
	if (temperature > 20 && temperature <= 35) {
		digitalWrite(Pins::GREEN, HIGH);
	}
	else {
		digitalWrite(Pins::GREEN, LOW);
	}

	// Control red LED
	if (temperature > 35) {
		digitalWrite(Pins::RED, HIGH);
	}
	else {
		digitalWrite(Pins::RED, LOW);
	}
}

void setup() {
	Serial.begin(9600);

	adc.begin();
	adc.setGain(GAIN_ONE);

	pinMode(Pins::RED, OUTPUT);
	pinMode(Pins::GREEN, OUTPUT);
	pinMode(Pins::BLUE, OUTPUT);
}

void loop() {
	int analogValue = adc.readADC_SingleEnded(Pins::THERMISTOR);
	analogAccumulator.addValue(analogValue);  // Average together past readings

	if (analogAccumulator.hasEnoughData) {
		// Enough data accumulated for calculations below

		double avgAnalogValue = analogAccumulator.getAvgValue();

		// Scale [0, ADC_MAX_VALUE] to [0, ADC_MAX_VOLTAGE]
		double thermVoltage = ADC_MAX_VOLTAGE * (avgAnalogValue / ADC_MAX_VALUE);

		// KVL: V_OUT - fixedResistorVoltage - thermVoltage = 0
		double fixedResistorVoltage = GlobalConstants::V_OUT - thermVoltage;

		// I = V / R
		double current = fixedResistorVoltage / GlobalConstants::FIXED_RESISTANCE;

		// R = V / I
		double thermResistance = thermVoltage / current;

		double temperature = calibrator.getTemperature(thermVoltage);
		double roundedTemperature = round(temperature * 10) / 10.0;

		Serial.println(
			"DATA | Analog Value: " + String(avgAnalogValue, 2) +
			" | Resistor Voltage (Volts): " + String(fixedResistorVoltage, 2) +
			" | Thermistor Voltage (Volts): " + String(thermVoltage, 2) +
			" | Thermistor Resistance (Ohms): " + String(thermResistance, 2) +
			" | Temperature (deg. C): " + String(roundedTemperature, 1)
		);

		inputHandler.poll(thermResistance);

		controlLEDs(temperature);
	}
	else {
		Serial.println("DATA");

		digitalWrite(Pins::RED, LOW);
		digitalWrite(Pins::GREEN, LOW);
		digitalWrite(Pins::BLUE, LOW);
	}

	delay(50);
}
