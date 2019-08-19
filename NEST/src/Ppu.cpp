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

unsigned char Ppu::getPPURegisterNameTableSetting() {
    //Base Name Table Address Setting
    //0: 0x2000, 1: 0x2400, 2: 0x2800, 3: 0x2C00
    return (unsigned char)(getPPURegister() & 0b00000011);
}

unsigned char Ppu::getPPURegisterVRAMIncrement() {
    //PPU Write Address Increment per read/write:
    //0: Increment of 1 (left to right)
    //1: Increment of 32 (traverses downward)

    return (unsigned char)(((getPPURegister() & 0b00000100) != 0) ? 32 : 1);
}

bool Ppu::getPPURegisterSpritePatternTableSetting() {
    //Pattern Table address for 8x8 sprites
    //0: 0x0000, 1: 0x1000
    return (getPPURegister() & 0b0001000) != 0;
}

bool Ppu::getPPURegisterBackgroundPatternTableSetting() {
    //Pattern Table address for background
    //0: 0x0000, 1: 0x1000
    return (getPPURegister() & 0b00010000) != 0;
}

bool Ppu::getPPURegisterSpriteSizeSetting() {
    //Sprite size Setting
    //0: 8x8 Sprites, 1: 8x16 Sprites
    return (getPPURegister() & 0b00100000) != 0;
}

bool Ppu::getPPURegisterNMISetting() {
    //Generate NMI at V-Blank setting
    //0: False
    //1: True
    return (getPPURegister() & 0b10000000) != 0;
}

unsigned char Ppu::getPPUMask() {
    return cpu.readCPURam(0x2001, true);
}

bool Ppu::getMaskGreyscaleEnabled() {
    return (getPPUMask() & 0b1) != 0;
}

bool Ppu::getMaskLeft8BGPixelsEnabled() {
    return (getPPUMask() & 0b10) != 0;
}

bool Ppu::getMaskLeft8SpritesPixelsEnabled() {
    return (getPPUMask() & 0b100) != 0;
}

bool Ppu::getMaskBackgroundEnabled() {
    return (getPPUMask() & 0b1000) != 0;
}

bool Ppu::getMaskSpriteEnabled() {
    return (getPPUMask() & 0b10000) != 0;
}

bool Ppu::getMaskEmphasizeRedEnabled() {
    return (getPPUMask() & 0b100000) != 0;
}

bool Ppu::getMaskEmphasizeGreenEnabled() {
    return (getPPUMask() & 0b1000000) != 0;
}

bool Ppu::getMaskEmphasizeBlueEnabled() {
    return (getPPUMask() & 0b10000000) != 0;
}
