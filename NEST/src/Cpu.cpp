#include "Core.h"
#include "Cpu.h"
#include "Binary.h"
#include "Rom.h"
Cpu::Cpu(Input& input)
    : input(input)
{
}

uchar Cpu::readImmediateByte() {
    return readCPURam(programCounter++);
}

ushort Cpu::readImmediateUShort() {
    return (ushort)(readImmediateByte() | (readImmediateByte() << 8));
}

uchar Cpu::zeroPageIndexed(uchar argument, uchar index, uchar offset) {
    //d, x  //d, y
    return readCPURam((ushort)((argument + index + offset) % 256));
}

uchar Cpu::absolute(ushort address, uchar offset) {
    return readCPURam((ushort)(address + offset));
}

uchar Cpu::absoluteIndexed(uchar argument, uchar index) {
    //a, x //a, y
    return readCPURam((ushort)(argument + index));
}

uchar Cpu::indexedIndirect(uchar argument) {
    //(d, x)
    ushort addressLower = readCPURam((ushort)((argument + xAddress) % 256));
    ushort addressUpper = (ushort)(readCPURam((ushort)((argument + xAddress + 1) % 256)) << 8);
    return readCPURam((ushort)(addressLower | addressUpper));

    //May need extra logic for if argument is 0xFF, and the next byte would be at 00?
}

void Cpu::writeIndexedIndirect(uchar address, uchar data) {
    //(d, x)
    ushort addressLower = readCPURam((ushort)((address + xAddress) % 256));
    ushort addressUpper = (ushort)(readCPURam((ushort)((address + xAddress + 1) % 256)) << 8);
    writeCPURam((ushort)(addressLower | addressUpper), data);

    //May need extra logic for if argument is 0xFF, and the next byte would be at 00?
}


uchar Cpu::indirectIndexed(uchar argument) {
    //(d), y
    ushort a = readCPURam(argument);
    ushort b = (ushort)(zeroPageIndexed(argument, 0, 1) << 8);
    return readCPURam((ushort)((a | b) + yAddress));
}

void Cpu::writeIndirectIndexed(uchar address, uchar data) {
    ushort a = readCPURam(address);
    ushort b = (ushort)(zeroPageIndexed(address, 0, 1) << 8);
    writeCPURam((ushort)((a | b) + yAddress), data);
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

void Cpu::opcode00() {
    //BRK: Force Interrupt

    readImmediateByte(); // Empty padding byte.
    setFlagTo(Breakpoint_Flag, true);
    pushStackU16(programCounter);
    pushStackU8(statusRegister);

    ushort address = readCPURam(0xFFFE);
    address |= (ushort)(readCPURam(0xFFFF) << 8);

    programCounter = --address;

    //7 Cycles
}

/*
 * @Name: setFlagTo
 * @Params: byte flag: This contains the bits representing the flag to modify status with.
 * @Params: bool enable: This tells whether to enable or disable this particular flag.
 * @Purpose: Allows enabling and disabling specific flags in the CPU status register.
 */
void Cpu::setFlagTo(uchar flag, bool enable) {
    if (enable) {
        statusRegister |= flag;
    } else {
        statusRegister &= (uchar)(~flag);
    }
}

/*
 * @Name: getFlagStatus
 * @Params: byte flag: This contains the bits to check in the status register.
 * @Purpose: Allows checking if a specific flag bit is enabled or disabled in the status register.
 */
bool Cpu::getFlagStatus(uchar flag) {
    return (statusRegister & flag) == flag;
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
