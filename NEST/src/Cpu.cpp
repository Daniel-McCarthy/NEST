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

void Cpu::opcode01() {
    //Bitwise OR A Indexed Indirect X

    uchar value = indexedIndirect(readImmediateByte());
    accumulator = ((uchar)(accumulator | value));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode05() {
    //Bitwise OR A Zero Page

    accumulator = ((uchar)(accumulator | readCPURam(readImmediateByte())));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode06() {
    //Bitwise Left Shift of Zero Page Value

    ushort address = readImmediateByte();
    uchar value = readCPURam(address);

    setFlagTo(Carry_Flag, (value & 0x80) == 0x80);          //Set carry flag to old bit 7

    value <<= 1;
    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode09() {
    //Bitwise OR A Immediate Byte

    accumulator = ((uchar)(accumulator | readImmediateByte()));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode0A() {
    //Bitwise Left Shift of Accumulator

    //Set carry flag to old bit 7
    setFlagTo(Carry_Flag, (accumulator & 0x80) == 0x80);
    accumulator <<= 1;

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode0D() {
    //Bitwise OR A Absolute 16 Bit Address

    accumulator = (uchar)(accumulator | absolute(readImmediateUShort()));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode0E() {
    //Bitwise Left Shift of value at absolute address

    ushort address = readImmediateUShort();
    unsigned char value = readCPURam(address);

    setFlagTo(Carry_Flag, (value & 0x80) == 0x80);          //Set carry flag to old bit 7

    value <<= 1;
    writeCPURam(address, (unsigned char)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode11() {
    //Bitwise OR A Indirect Indexed Y

    uchar value = indirectIndexed(readCPURam(programCounter++));
    accumulator = ((uchar)(accumulator | value));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode15() {
    //Bitwise OR A Zero Page X

    accumulator = ((uchar)(accumulator | zeroPageIndexed(readImmediateByte(), xAddress)));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode16() {
    //Bitwise Left Shift of value at Zero Page X address

    ushort address = (ushort)(readImmediateByte() + xAddress);
    uchar value = readCPURam(address);

    setFlagTo(Carry_Flag, (value & 0x80) == 0x80);          //Set carry flag to old bit 7

    value <<= 1;
    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode19() {
    //Bitwise OR A Absolute Y Index 16 Bit Address

    accumulator = (uchar)(accumulator | absolute(readImmediateUShort(), yAddress));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode1D() {
    //Bitwise OR A Absolute X Index 16 Bit Address

    accumulator = (uchar)(accumulator | absolute(readImmediateUShort(), xAddress));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode1E() {
    //Bitwise Left Shift of value at absolute X address

    ushort address = readImmediateUShort();
    address += xAddress;
    uchar value = readCPURam(address);

    setFlagTo(Carry_Flag, (value & 0x80) == 0x80);          //Set carry flag to old bit 7

    value <<= 1;
    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode21() {
    //Bitwise And A with Indexed Indirect X

    uchar value = indexedIndirect(readImmediateByte());
    accumulator = ((uchar)(accumulator & value));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode24() {
    //Bitwise Test of Zero Page value with Bit Mask in Accumulator

    ushort address = readImmediateByte();

    //Store bit 7 and 6 in Negative and Overflow flags respectively.
    uchar value = readCPURam(address);
    setFlagTo(Negative_Flag, (value & 0x80) != 0);
    setFlagTo(Overflow_Flag, (value & 0x40) != 0);

    value &= accumulator;
    setFlagTo(Zero_Flag, (value == 0));

    // 3 cycles total. Read opcode byte, operand byte, and read value from address.
}

void Cpu::opcode25() {
    //Bitwise And A with Zero Page Immediate Byte

    accumulator = ((uchar)(accumulator & readCPURam(readImmediateByte())));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode26() {
    //Bitwise Left Rotate of Zero Page Value

    ushort address = readImmediateByte();
    uchar value = readCPURam(address);

    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (value & 0x80) == 0x80);          //Set carry flag to old bit 7

    value <<= 1;

    if (oldCarry)
    {
        value |= 0x1;
    }

    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode29() {
    //Bitwise And A with Immediate Byte

    accumulator = ((uchar)(accumulator & readImmediateByte()));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode2A() {
    //Bitwise Left Rotate of Accumulator

    //Set carry flag to old bit 7
    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (accumulator & 0x80) == 0x80);
    accumulator <<= 1;

    //Set new bit 0 to previous carry flag
    if (oldCarry)
    {
        accumulator |= 0x1;
    }

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode2C() {
    //Bitwise Test of value at absolute address with Bit Mask in Accumulator

    ushort address = readImmediateUShort();

    //Store bit 7 and 6 in Negative and Overflow flags respectively.
    uchar value = readCPURam(address);
    setFlagTo(Negative_Flag, (value & 0x80) != 0);
    setFlagTo(Overflow_Flag, (value & 0x40) != 0);

    //Mask value with accumulator value and set Zero flag
    value &= accumulator;
    setFlagTo(Zero_Flag, (value == 0));

    // 4 cycles total. Read opcode byte, 2 operand bytes, and read value from address.
}

void Cpu::opcode2D() {
    //Bitwise And A with absolute 16 bit Address

    accumulator = ((uchar)(accumulator & absolute(readImmediateUShort())));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode2E() {
    //Bitwise Left Rotate of value at absolute address

    ushort address = readImmediateUShort();
    uchar value = readCPURam(address);

    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (value & 0x80) == 0x80);          //Set carry flag to old bit 7

    value <<= 1;

    if (oldCarry)
    {
        value |= 0x01;
    }

    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode31() {
    //Bitwise And A Indirect Indexed Y

    uchar value = indirectIndexed(readCPURam(programCounter++));
    accumulator = ((uchar)(accumulator & value));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode35() {
    //Bitwise And A with Zero Page X

    uchar value = zeroPageIndexed(readImmediateByte(), xAddress);
    accumulator = ((uchar)(accumulator & value));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode36() {
    //Bitwise Left Rotate of value at Zero Page X address

    ushort address = (ushort)(readImmediateByte() + xAddress);
    uchar value = readCPURam(address);

    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (value & 0x80) == 0x80);          //Set carry flag to old bit 7

    value <<= 1;

    if (oldCarry)
    {
        value |= 0x01;
    }

    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode39() {
    //Bitwise And A with Absolute + Y Address

    uchar value = absolute(readImmediateUShort(), yAddress);
    accumulator = ((uchar)(accumulator & value));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode3D() {
    //Bitwise And A with Absolute + X Address

    uchar value = absolute(readImmediateUShort(), xAddress);
    accumulator = ((uchar)(accumulator & value));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode3E() {
    //Bitwise Left Rotate of value at absolute X address

    ushort address = (ushort)(readImmediateUShort() + xAddress);
    uchar value = readCPURam(address);

    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (value & 0x80) == 0x80);          //Set carry flag to old bit 7

    value <<= 1;

    if (oldCarry)
    {
        value |= 0x01;
    }

    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode41() {
    //Bitwise XOR A Indexed Indirect X

    uchar value = indexedIndirect(readCPURam(programCounter++));
    accumulator = ((uchar)(accumulator ^ value));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode45() {
    //Bitwise XOR A with Zero Page address

    accumulator = ((uchar)(accumulator ^ zeroPageIndexed(readImmediateByte(),0)));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode46() {
    //Bitwise Right Shift of Zero Page Value

    ushort address = readImmediateByte();
    uchar value = readCPURam(address);

    setFlagTo(Carry_Flag, (value & 0x01) == 0x01);          //Set carry flag to old bit 7

    value >>= 1;
    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode49() {
    //Bitwise XOR A with Immediate byte

    accumulator = (uchar)(accumulator ^ readImmediateByte());

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode4A() {
    //Bitwise Right Shift of Accumulator

    //Set carry flag to old bit 7
    setFlagTo(Carry_Flag, (accumulator & 0x01) == 0x01);
    accumulator >>= 1;

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode4D() {
    //Bitwise XOR A with absolute address

    accumulator = (uchar)(accumulator ^ readCPURam(readImmediateUShort()));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode4E() {
    //Bitwise Right Shift of value at absolute address

    ushort address = readImmediateUShort();
    uchar value = readCPURam(address);

    setFlagTo(Carry_Flag, (value & 0x01) == 0x01);          //Set carry flag to old bit 7

    value >>= 1;
    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode51() {
    //Bitwise XOR A Indirect Indexed Y

    uchar value = indirectIndexed(readCPURam(programCounter++));
    accumulator = ((uchar)(accumulator ^ value));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode55() {
    //Bitwise XOR A with Zero Page X address

    accumulator = ((uchar)(accumulator ^ zeroPageIndexed(readImmediateByte(), xAddress)));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode56() {
    //Bitwise Right Shift of value at Zero Page X address

    ushort address = (ushort)(readImmediateByte() + xAddress);
    uchar value = readCPURam(address);

    setFlagTo(Carry_Flag, (value & 0x01) == 0x01);          //Set carry flag to old bit 7

    value >>= 1;
    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode59() {
    //Bitwise XOR A with Absolute Y address

    accumulator = (uchar)(accumulator ^ absolute(readImmediateUShort(), yAddress));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode5D() {
    //Bitwise XOR A with Absolute X address

    accumulator = (uchar)(accumulator ^ absolute(readImmediateUShort(), xAddress));

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 1;
    tClock += 4;
}

void Cpu::opcode5E() {
    //Bitwise Right Shift of value at absolute X address

    ushort address = readImmediateUShort();
    address += xAddress;
    uchar value = readCPURam(address);

    setFlagTo(Carry_Flag, (value & 0x01) == 0x01);          //Set carry flag to old bit 7

    value >>= 1;
    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode66() {
    //Bitwise Right Rotate of Zero Page Value

    ushort address = readImmediateByte();
    uchar value = readCPURam(address);

    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (value & 0x01) == 0x01);          //Set carry flag to old bit 7

    value >>= 1;

    if (oldCarry)
    {
        value |= 0x80;
    }

    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode6A() {
    //Bitwise Right Rotate of Accumulator

    //Set carry flag to old bit 0
    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (accumulator & 0x01) == 0x01);
    accumulator >>= 1;

    //Set new bit 7 to previous carry flag
    if (oldCarry)
    {
        accumulator |= 0x80;
    }

    setFlagTo(Zero_Flag, (accumulator == 0));
    setFlagTo(Negative_Flag, (accumulator & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode6E() {
    //Bitwise Right Rotate of value at absolute address

    ushort address = readImmediateUShort();
    uchar value = readCPURam(address);

    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (value & 0x01) == 0x01);          //Set carry flag to old bit 7

    value >>= 1;

    if(oldCarry)
    {
        value |= 0x80;
    }

    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode76() {
    //Bitwise Right Rotate of value at Zero Page X address

    ushort address = (ushort)(readImmediateByte() + xAddress);
    uchar value = readCPURam(address);

    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (value & 0x01) == 0x01);          //Set carry flag to old bit 7

    value >>= 1;

    if (oldCarry)
    {
        value |= 0x80;
    }

    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
}

void Cpu::opcode7E() {
    //Bitwise Right Rotate of value at absolute X address

    ushort address = (ushort)(readImmediateUShort() + xAddress);
    uchar value = readCPURam(address);

    bool oldCarry = getFlagStatus(Carry_Flag);
    setFlagTo(Carry_Flag, (value & 0x01) == 0x01);          //Set carry flag to old bit 7

    value >>= 1;

    if (oldCarry)
    {
        value |= 0x80;
    }

    writeCPURam(address, (uchar)(value));

    setFlagTo(Zero_Flag, (value == 0));
    setFlagTo(Negative_Flag, (value & 0x80) != 0);

    mClock += 2;
    tClock += 8;
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
