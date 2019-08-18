#include "Ppu.h"
#include "Core.h"

Ppu::Ppu()
{
}

Ppu::~Ppu()
{
}

unsigned char Ppu::readPPURamByte(ushort address) {
    return ppuRam[address];
}

void Ppu::writePPURamByte(ushort address, unsigned char value) {
    ppuRam[address] = value;
}
