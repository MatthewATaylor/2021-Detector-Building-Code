#include "TempAccumulator.h"

int TempAccumulator::compare(const void *num1, const void *num2) {
    return static_cast<int>(
        *static_cast<const double*>(num1) - *static_cast<const double*>(num2)
    );
}

double TempAccumulator::getAvgTemp() {
    // Sort previous temperatures from smallest to largest
    double sortedPrevTemperatures[NUM_TEMPERATURES];
    for (int i = 0; i < NUM_TEMPERATURES; ++i) {
        sortedPrevTemperatures[i] = prevTemperatures[i];
    }
    qsort(sortedPrevTemperatures, NUM_TEMPERATURES, sizeof(double), compare);

    // Average together middle NUM_AVERAGED_TEMPERATURES temperatures
    double temperatureSum = 0;
    int numExcludedExtremes = (NUM_TEMPERATURES - NUM_AVERAGED_TEMPERATURES) / 2;
    for (int i = numExcludedExtremes; i < NUM_TEMPERATURES - numExcludedExtremes; ++i) {
        temperatureSum += sortedPrevTemperatures[i];
    }
    return temperatureSum / NUM_AVERAGED_TEMPERATURES;
}

String TempAccumulator::getAvgTempStr(int decimalPlaces) {
    return canDisplayTemp ? String(getAvgTemp(), decimalPlaces) : "Not enough data";
}

void TempAccumulator::addTemperature(double temperature) {
    // Update prevTemps buffer with new temperature reading
    prevTemperatures[newTemperatureIndex] = temperature;
    if (++newTemperatureIndex >= NUM_TEMPERATURES) {
        newTemperatureIndex = 0;
    }
    if (!canDisplayTemp) {
        if (++canDisplayCounter >= NUM_TEMPERATURES) {
            // Only display temperature following NUM_TEMPS readings
            canDisplayTemp = true;
        }
    }
}
