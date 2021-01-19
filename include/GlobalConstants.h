#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

#include <Arduino.h>

struct GlobalConstants {
	static constexpr double KELVIN_CONVERSION = 273.15;
	static constexpr double V_OUT = 3.3;

	// Theoretical fixed resistance: 10000 ohms
	// Resistance determined with multimeter (9940 ohms) is used
	static constexpr double FIXED_RESISTANCE = 9940;
};

#endif
