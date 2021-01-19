template <typename T>
inline int Accumulator<T>::compare(const void *num1, const void *num2) {
    return static_cast<int>(
        *static_cast<const T*>(num1) - *static_cast<const T*>(num2)
    );
}

template <typename T>
inline double Accumulator<T>::getAvgValue() {
    // Sort previous values from smallest to largest
    T sortedPrevValues[NUM_VALUES];
    for (int i = 0; i < NUM_VALUES; ++i) {
        sortedPrevValues[i] = prevValues[i];
    }
    qsort(sortedPrevValues, NUM_VALUES, sizeof(T), compare);

    // Average together middle NUM_AVERAGED_VALUES values
    double valueSum = 0.0;
    int numExcludedExtremes = (NUM_VALUES - NUM_AVERAGED_VALUES) / 2;
    for (int i = numExcludedExtremes; i < NUM_VALUES - numExcludedExtremes; ++i) {
        valueSum += sortedPrevValues[i];
    }
    return valueSum / static_cast<double>(NUM_AVERAGED_VALUES);
}

template <typename T>
inline void Accumulator<T>::addValue(T value) {
    // Update prevValues buffer with new value reading
    prevValues[newValueIndex] = value;
    if (++newValueIndex >= NUM_VALUES) {
        newValueIndex = 0;
    }
    if (!hasEnoughData) {
        if (++enoughDataCounter >= NUM_VALUES) {
            // Only display value following NUM_VALUES readings
            hasEnoughData = true;
        }
    }
}
