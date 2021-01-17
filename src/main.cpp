#include <Arduino.h>
#include <Adafruit_ADS1015.h>

#include "TempAccumulator.h"
#include "Calibrator.h"
#include "DataWriter.h"
#include "InputHandler.h"
#include "GlobalConstants.h"

// 16-bit ADC
const double ADC_MAX = 65535.0;

// Digital and analog IO pin numbers
struct Pins {
	const static int THERMISTOR = 0;
	const static int RED = 2;
	const static int GREEN = 3;
	const static int BLUE = 4;
};

Adafruit_ADS1115 adc;
TempAccumulator accumulator;
Calibrator calibrator;
DataWriter dataWriter;
InputHandler inputHandler(&calibrator, &dataWriter);

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

	pinMode(Pins::RED, OUTPUT);
	pinMode(Pins::GREEN, OUTPUT);
	pinMode(Pins::BLUE, OUTPUT);
}

void loop() {
	int analogValue = adc.readADC_SingleEnded(Pins::THERMISTOR);

	// Scale [0, ADC_MAX] to [0, V_OUT]
	double thermVoltage = GlobalConstants::V_OUT * (analogValue / ADC_MAX);

	// KVL: V_OUT - fixedResistorVoltage - thermVoltage = 0
	double fixedResistorVoltage = GlobalConstants::V_OUT - thermVoltage;

	// I = V / R
	double current = fixedResistorVoltage / GlobalConstants::FIXED_RESISTANCE;

	// R = V / I
	double thermResistance = thermVoltage / current;

	double temperature = calibrator.getTemperature(thermVoltage);

	// Read commands entered through serial monitor
	inputHandler.poll(thermResistance);

	// Add temperature into accumulator for averaging with previous values
	accumulator.addTemperature(temperature);

	// Display data on serial monitor
	dataWriter.write(
		analogValue, fixedResistorVoltage, thermVoltage,
		thermResistance, &accumulator
	);

	controlLEDs(temperature);

	delay(50);
}
