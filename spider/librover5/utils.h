#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

template <uint8_t size, typename TReading> class CRollingAverage
{
    TReading data[size];
    uint32_t dataSum;
    uint8_t index, dataCount;

public:
    CRollingAverage(void) { reset(); }

    void add(TReading r)
    {
        dataSum -= data[index];
        data[index] = r;
        dataSum += data[index];

        ++index;
        if (index >= size)
            index = 0;

        ++dataCount;
    }

    void reset(void) { memset(data, 0, sizeof(data)); dataSum = index = dataCount = 0; }
    TReading average(void) const { return dataSum / min(dataCount, size); }
    uint8_t getCount(void) const { return dataCount; }
};

#endif // UTILS_H
