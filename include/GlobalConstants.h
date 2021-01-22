#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

#include <Arduino.h>

struct GlobalConstants {
	static constexpr double KELVIN_CONVERSION = 273.15;
	static constexpr double V_OUT = 3.3;
	static constexpr double FIXED_RESISTANCE = 220000;
};

#endif
