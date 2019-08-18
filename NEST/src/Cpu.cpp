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
{
}

