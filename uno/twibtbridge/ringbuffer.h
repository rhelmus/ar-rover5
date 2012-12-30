#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>

// Simple ringbuffer, inspired by Dean Camera's ringbuffer example:
// http://www.fourwalledcubicle.com/AVRCodeSamples.php

template <int size> class CRingBuffer
{
    uint8_t data[size];
    uint16_t inputIndex, outputIndex;
    uint16_t count;

public:
    CRingBuffer(void) : inputIndex(0), outputIndex(0), count(0) { }

    void push(uint8_t b) volatile
    {
        data[inputIndex] = b;
        if (inputIndex == (size-1))
            inputIndex = 0; // Wrap to beginning
        else
            ++inputIndex;

        if (!isFull())
            ++count;
    }

    uint8_t pop(void) volatile
    {
        const uint8_t ret = data[outputIndex];
        if (outputIndex == (size-1))
            outputIndex = 0; // Wrap to beginning
        else
            ++outputIndex;
        --count;
        return ret;
    }

    uint16_t getCount(void) const volatile { return count; }
    bool isFull(void) const volatile { return (count == size); }
    bool isEmpty(void) const volatile { return (count == 0); }
};

#endif // RINGBUFFER_H
