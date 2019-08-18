#include "Binary.h"

unsigned char rotateLeft(unsigned char number)
{
    unsigned char lostBit = (number & 0x80) >> 7;
    number <<= 1;
    number |= lostBit;

    return number;
}

unsigned char rotateLeft(unsigned char number, int shiftAmount)
{
    for (int i = 0; i < shiftAmount; i++)
    {
        number = rotateLeft(number);
    }

    return number;
}

unsigned char rotateRight(unsigned char number)
{
    unsigned char lostBit = ((number & 0x01) << 7) & 0xFF;
    number >>= 1;
    number |= lostBit;

    return number;
}

unsigned char rotateRight(unsigned char number, int shiftAmount)
{
    for (int i = 0; i < shiftAmount; i++)
    {
        number = rotateRight(number);
    }

    return number;
}
