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
    void pushStackU8(uchar value);
    void pushStackU16(ushort value);
    uchar popStackU8();
    ushort popStackU16();
    bool detectADCOverflow(int value, int addition, int sum);
    bool detectSBCOverflow(int value, int addition, int sum);

public slots:
};

#endif // !CPUH
