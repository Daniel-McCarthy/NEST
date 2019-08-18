#pragma once

#include <QWidget>

#ifndef CPUH
#define CPUH

class Core;
class Input;
class Cpu : public QWidget
{
    Q_OBJECT

private:

    //NEST CPU Registers
    unsigned char accumulator = 0;                                          //Contains results of arithmetic functions
    unsigned char xAddress = 0;                                             //X Index Value
    unsigned char yAddress = 0;                                             //Y Index Value
    unsigned char stackPointer = 0xFD;                                      //Tracks position in the stack
    unsigned char statusRegister = 0 | Empty_Flag | Interrupt_Disable_Flag; //Tracks what flags are set in the CPU

    const unsigned char Carry_Flag               = 0x1;          //C
    const unsigned char Zero_Flag                = 0x1 << 1;     //Z
    const unsigned char Interrupt_Disable_Flag   = 0x1 << 2;     //I
    const unsigned char Decimal_Mode_Flag        = 0x1 << 3;     //D
    const unsigned char Breakpoint_Flag          = 0x1 << 4;     //B
    const unsigned char Empty_Flag               = 0x1 << 5;     //-
    const unsigned char Overflow_Flag            = 0x1 << 6;     //V
    const unsigned char Negative_Flag            = 0x1 << 7;     //S

    const unsigned short PPU_CONTROL_REGISTER       = 0x2000;
    const unsigned short PPU_MASK_REGISTER          = 0x2001;
    const unsigned short PPU_STATUS_REGISTER        = 0x2002;
    const unsigned short OAM_DATA_ADDRESS_REGISTER  = 0x2003;
    const unsigned short OAM_DATA_REGISTER          = 0x2004;
    const unsigned short PPU_SCROLL_REGISTER        = 0x2005;
    const unsigned short PPU_DATA_ADDRESS_REGISTER  = 0x2006;
    const unsigned short PPU_DATA_REGISTER          = 0x2007;
    const unsigned short OAM_DMA_REGISTER           = 0x4014;
    const unsigned short JOYPAD1_REGISTER           = 0x4016;
    const unsigned short JOYPAD2_REGISTER           = 0x4017;

    // CPU Ram Map:
    // 0x0000-0x07FF: Internal Ram
    // 0x0800-0x1FFF: Mirroring of Internal Ram
    // 0x2000-0x2007: PPU Registers
    // 0x2008-0x3FFF: Mirroring of PPU Registers
    // 0x4000-0x4017: APU & I/O Registers
    // 0x4018-0x401F: APU & I/O Functionality
    // 0x4020-0xFFFF: Cartridge Space
    unsigned char cpuRam[0x10000];

    Input& input;

    void opcode00();
    void opcode01();
    void opcode04();
    void opcode05();
    void opcode06();
    void opcode08();
    void opcode09();
    void opcode0A();
    void opcode0C();
    void opcode0D();
    void opcode0E();
    void opcode10();
    void opcode11();
    void opcode14();
    void opcode15();
    void opcode16();
    void opcode18();
    void opcode19();
    void opcode1E();
    void opcode1D();
    void opcode20();
    void opcode21();
    void opcode24();
    void opcode25();
    void opcode26();
    void opcode28();
    void opcode29();
    void opcode2A();
    void opcode2C();
    void opcode2D();
    void opcode2E();
    void opcode30();
    void opcode31();
    void opcode34();
    void opcode35();
    void opcode36();
    void opcode38();
    void opcode39();
    void opcode3D();
    void opcode3E();
    void opcode41();
    void opcode44();
    void opcode45();
    void opcode46();
    void opcode48();
    void opcode49();
    void opcode4C();
    void opcode4D();
    void opcode4A();
    void opcode4E();
    void opcode50();
    void opcode51();
    void opcode54();
    void opcode55();
    void opcode56();
    void opcode58();
    void opcode59();
    void opcode5D();
    void opcode5E();
    void opcode64();
    void opcode66();
    void opcode68();
    void opcode6A();
    void opcode6C();
    void opcode6E();
    void opcode70();
    void opcode74();
    void opcode76();
    void opcode78();
    void opcode7E();
    void opcode80();
    void opcode82();
    void opcode89();
    void opcode90();
    void opcodeB0();
    void opcodeB8();
    void opcodeC2();
    void opcodeD0();
    void opcodeD4();
    void opcodeD8();
    void opcodeE2();
    void opcodeEA();
    void opcodeF0();
    void opcodeF4();

public:
    unsigned short programCounter = 0x8000;                                 //Tracks position in the program
    bool pendingInterrupt = false;
    uint tClock = 0;
    uint mClock = 0;

    Cpu(Input& input);
    ~Cpu() {}

	int returnTClock();
	int returnMClock();
    unsigned char readCPURam(ushort address, bool ignoreCycles = false);
    void writeCPURam(ushort address, uchar value, bool ignoreCycles = false);
    void  directCPURamWrite(ushort address, uchar value);
    uchar readImmediateByte();
    ushort readImmediateUShort();

    uchar zeroPageIndexed(uchar argument, uchar index, uchar offset = 0);
    uchar absolute(ushort address, uchar offset = 0);
    uchar absoluteIndexed(uchar argument, uchar index);
    uchar indexedIndirect(uchar argument);
    void writeIndexedIndirect(uchar address, uchar data);
    uchar indirectIndexed(uchar argument);
    void writeIndirectIndexed(uchar address, uchar data);

    void pushStackU8(uchar value);
    void pushStackU16(ushort value);
    uchar popStackU8();
    ushort popStackU16();
    void setFlagTo(uchar flag, bool enable);
    bool getFlagStatus(uchar flag);
    bool detectADCOverflow(int value, int addition, int sum);
    bool detectSBCOverflow(int value, int addition, int sum);
    uchar getAccumulator();
    uchar getStackPointer();
    uchar getStatus();
    ushort getProgramCounter();
    uchar getXRegister();
    uchar getYRegister();
    void setStackPointer(uchar newSP);

public slots:
};

#endif // !CPUH
