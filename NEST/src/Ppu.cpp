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

unsigned char Ppu::readOAMRamByte(ushort address) {
    return oamRam[address];
}

void Ppu::writeOAMRamByte(ushort address, unsigned char value) {
    oamRam[address] = value;
}
