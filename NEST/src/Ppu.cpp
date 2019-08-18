#include "Ppu.h"
#include "Cpu.h"

Ppu::Ppu(Cpu& cpu)
    : cpu(cpu)
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

unsigned char Ppu::getPPUStatus() {
    unsigned char ppuStatus = 0;

    unsigned char PPU_STATE_VBLANK = 0b00000011;
    if (ppuState == PPU_STATE_VBLANK) {
        ppuStatus |= 0x80;
    }

    if (spriteZeroHit) {
        ppuStatus |= 0x40;
    }

    if (spriteOverflow) {
        ppuStatus |= 0x20;
    }

    //TODO: Bitwise Or the least significant bits of last byte written into PPU register

    return ppuStatus;
}

unsigned char Ppu::getPPURegister() {
    return cpu.readCPURam(0x2000, true);
}

