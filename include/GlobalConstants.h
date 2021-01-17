#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

struct GlobalConstants {
	static constexpr double KELVIN_CONVERSION = 273.15;
	static constexpr double V_OUT = 3.3;

	// Theoretical fixed resistance: 100000 ohms
	// Resistance determined with multimeter (101000 ohms) is used
	static constexpr double FIXED_RESISTANCE = 101000;
};

#endif
