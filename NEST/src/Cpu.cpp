#include "Core.h"
#include "Cpu.h"
#include "Binary.h"
#include "Rom.h"
Cpu::Cpu(Input& input)
    : input(input)
{
}

unsigned char Cpu::readCPURam(ushort address, bool ignoreCycles)
{
    if(!ignoreCycles)
    {
        mClock += 1;
        tClock += 4;
    }

    if (address == PPU_STATUS_REGISTER) {
        // To be implemented with PPU

    } else if (address == PPU_DATA_REGISTER) {
        // To be implemented with PPU

    } else if (address == JOYPAD1_REGISTER) {
        return input.joyPadRegisterRead();
    }  else {
        return cpuRam[address];
    }
}

void Cpu::writeCPURam(ushort address, uchar value, bool ignoreCycles) {
    if (!ignoreCycles) {
        mClock += 1;
        tClock += 4;
    }
    if (address == OAM_DMA_REGISTER) {
        // To be implemented with PPU
    } else if (address == PPU_DATA_REGISTER) {
        // To be implemented with PPU
    } else if(address == PPU_DATA_ADDRESS_REGISTER) {
        // To be implemented with PPU
    } else if (address == OAM_DATA_REGISTER) {
        // To be implemented with PPU
    } else if(address == OAM_DATA_ADDRESS_REGISTER) {
        // To be implemented with PPU
    } else if (address == PPU_SCROLL_REGISTER) {
        // To be implemented with PPU
    } else if (address == JOYPAD1_REGISTER) {
        input.joyPadRegisterWrite(value);

    }  else {
        cpuRam[address] = value;
    }
}

void  Cpu::directCPURamWrite(ushort address, uchar value) {
    cpuRam[address] = value;
}

void Cpu::pushStackU8(uchar value) {
    writeCPURam((ushort)(0x100 | stackPointer), value);
    stackPointer--;
}

void Cpu::pushStackU16(ushort value) {
    pushStackU8((uchar)((value >> 8) & 0xFF));
    pushStackU8((uchar)(value & 0xFF));
}

uchar Cpu::popStackU8()
{
    stackPointer++;
    return readCPURam((ushort)(0x100 | stackPointer));
}

ushort Cpu::popStackU16() {
    return (ushort)(popStackU8() | (popStackU8() << 8));
}

bool Cpu::detectADCOverflow(int value, int addition, int sum) {
    return (!(((value ^ addition) & 0x80) > 0)) && (((value ^ sum) & 0x80) > 0);
}

bool Cpu::detectSBCOverflow(int value, int addition, int sum) {
    return ((((value ^ sum) & 0x80) > 0)) && (((value ^ addition) & 0x80) > 0);
}

uchar Cpu::getAccumulator() {
    return accumulator;
}

uchar Cpu::getStackPointer() {
    return stackPointer;
}

uchar Cpu::getStatus() {
    return statusRegister;
}

ushort Cpu::getProgramCounter() {
    return programCounter;
}

uchar Cpu::getXRegister() {
    return xAddress;
}

uchar Cpu::getYRegister() {
    return yAddress;
}

void Cpu::setStackPointer(uchar newSP)
{
    stackPointer = newSP;
}
