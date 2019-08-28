#pragma once

#include <QWidget>

#ifndef CPUH
#define CPUH

class Input;
class Ppu;
class Mapper;
class Cpu : public QWidget
{
    Q_OBJECT

private:
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

    //NEST CPU Registers
    unsigned char accumulator = 0;                                          //Contains results of arithmetic functions
    unsigned char xAddress = 0;                                             //X Index Value
    unsigned char yAddress = 0;                                             //Y Index Value
    unsigned char stackPointer = 0xFD;                                      //Tracks position in the stack
    unsigned char statusRegister = 0 | Empty_Flag | Interrupt_Disable_Flag; //Tracks what flags are set in the CPU

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
    Ppu& ppu;
    Mapper& mapper;

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
    void opcode40();
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
    void opcode60();
    void opcode61();
    void opcode64();
    void opcode65();
    void opcode66();
    void opcode68();
    void opcode69();
    void opcode6A();
    void opcode6C();
    void opcode6D();
    void opcode6E();
    void opcode70();
    void opcode71();
    void opcode74();
    void opcode75();
    void opcode76();
    void opcode78();
    void opcode79();
    void opcode7D();
    void opcode7E();
    void opcode80();
    void opcode81();
    void opcode82();
    void opcode84();
    void opcode85();
    void opcode86();
    void opcode88();
    void opcode89();
    void opcode8A();
    void opcode8C();
    void opcode8D();
    void opcode8E();
    void opcode90();
    void opcode91();
    void opcode94();
    void opcode95();
    void opcode96();
    void opcode98();
    void opcode99();
    void opcode9A();
    void opcode9D();
    void opcodeA0();
    void opcodeA1();
    void opcodeA2();
    void opcodeA4();
    void opcodeA5();
    void opcodeA6();
    void opcodeA8();
    void opcodeA9();
    void opcodeAA();
    void opcodeAC();
    void opcodeAD();
    void opcodeAE();
    void opcodeB0();
    void opcodeB1();
    void opcodeB4();
    void opcodeB5();
    void opcodeB6();
    void opcodeB8();
    void opcodeB9();
    void opcodeBA();
    void opcodeBC();
    void opcodeBD();
    void opcodeBE();
    void opcodeC0();
    void opcodeC1();
    void opcodeC2();
    void opcodeC4();
    void opcodeC5();
    void opcodeC6();
    void opcodeC8();
    void opcodeC9();
    void opcodeCA();
    void opcodeCC();
    void opcodeCD();
    void opcodeCE();
    void opcodeD0();
    void opcodeD1();
    void opcodeD4();
    void opcodeD5();
    void opcodeD6();
    void opcodeD8();
    void opcodeD9();
    void opcodeDD();
    void opcodeDE();
    void opcodeE0();
    void opcodeE1();
    void opcodeE2();
    void opcodeE4();
    void opcodeE5();
    void opcodeE6();
    void opcodeE8();
    void opcodeE9();
    void opcodeEA();
    void opcodeEC();
    void opcodeED();
    void opcodeEE();
    void opcodeF0();
    void opcodeF1();
    void opcodeF4();
    void opcodeF5();
    void opcodeF6();
    void opcodeF8();
    void opcodeF9();
    void opcodeFD();
    void opcodeFE();

public:
    unsigned short programCounter = 0x8000;                                 //Tracks position in the program
    bool pendingInterrupt = false;
    uint tClock = 0;
    uint mClock = 0;


    Cpu(Input& input, Ppu& ppu, Mapper& mapper);
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

    void fetchAndExecute();
    void serviceInterrupt();
    void serviceNonMaskableInterrupt();
    bool loadSaveFile(QByteArray saveFile);
    QByteArray returnSaveDataFromCpuRam();
    void resetCPU();
public slots:
};

#endif // !CPUH
