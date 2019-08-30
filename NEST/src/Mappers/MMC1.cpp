#include "MMC1.h"
#include "src/Rom.h"
#include "src/Ppu.h"
#include "src/Cpu.h"

MMC1::MMC1(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu) : QObject(parent), rom(rom), ppu(ppu), cpu(cpu)
{
}

void MMC1::loadRom() {
    if (rom.getMapperSetting() == 1) {
        loadPrgRomBank(0x8000, 0);
        loadPrgRomBank(0xC000, (unsigned char)(rom.getProgramRomSize() - 1));
        loadChrRomBank(0x0000, 0);
        loadChrRomBank(0x1000, 1);

        ppu.isNametableMirrored = false;
    }
}

bool MMC1::isMapperWriteAddress(ushort address) {
    return address >= 0x8000 && address <= 0xFFFF;
}

void MMC1::writeMMC1(ushort address, unsigned char value) {

    bool writeToMMC1ControlRegister     = address >= 0x8000 && address <= 0x9FFF;
    bool writeToMMC1RamPage1Register    = address >= 0xA000 && address <= 0xBFFF;
    bool writeToMMC1RamPage2Register    = address >= 0xC000 && address <= 0xDFFF;
    bool writeToMMC1RomPageRegister     = address >= 0xE000 && address <= 0xFFFF;

    if(address >= 0x8000 && address <= 0xFFFF) {
        if ((value & 0x80) == 0x80) {
            //Reset Shift if bit 7 is set
            writeRegisterShift = 0;
            writeRegisterValue = 0;
            controlRegisterValue |= 0x0C;

            //0x0C write locks 0xC000 to last bank of Prg Rom
            loadPrgRomBank(0xC000, (unsigned char)(rom.getProgramRamSize() - 1));
        } else {
            if (writeRegisterShift < 5) {
                writeRegisterValue |= (unsigned char)((value & 0x01) << writeRegisterShift);
                writeRegisterShift++;
            }

            if (writeRegisterShift == 5) {
                if (writeToMMC1ControlRegister) {
                    controlRegisterValue = writeRegisterValue;

                    unsigned char nameTableSetting = (unsigned char)(controlRegisterValue & 0b11);

                    if (nameTableSetting == 0) {
                        ppu.isNametableMirrored = false;
                        //TODO: Implement one-screen lower bank
                    } else if (nameTableSetting == 1) {
                        ppu.isNametableMirrored = false;
                        //TODO: Implement one-screen upper bank
                    } else if (nameTableSetting == 2) {
                        //Set to Vertical Name Table Mirroring Mode
                        ppu.isNametableMirrored = true;
                        ppu.isVertNametableMirror = true;
                        ppu.isHorizNametableMirror = false;
                    } else if (nameTableSetting == 3) {
                        //Set to Horizontal Name Table Mirroring Mode
                        ppu.isNametableMirrored = true;
                        ppu.isVertNametableMirror = false;
                        ppu.isHorizNametableMirror = true;

                    }

                    //Set prgRom Banking Mode
                    prgRomBankSwitchingMode = (unsigned char)((controlRegisterValue & 0b1100) >> 2);

                    //Set chrRom Banking Mode
                    chrRom8kbBankSwitchingMode = (controlRegisterValue & 0b10000) == 0;

                }  else if (writeToMMC1RamPage1Register) {
                    //Initiate CHR Rom Bank Swap
                    ramPage1RegisterValue = writeRegisterValue;

                    if (chrRom8kbBankSwitchingMode) {
                        //Even number for 8kb banks
                        ramPage1RegisterValue &= 0b000011110;
                    } else {
                        //Full number to select 4kb banks
                        ramPage1RegisterValue &= 0b000011111;
                    }

                    int numBanks = (rom.getCHRRomSize()) * ((chrRom8kbBankSwitchingMode) ? 1 : 2);
                    if (numBanks > 0 && ramPage1RegisterValue <= numBanks) {
                        loadChrRomBank(0x0000, ramPage1RegisterValue);
                    }
                }
                else if (writeToMMC1RamPage2Register)
                {
                    //Initiate CHR Rom Bank Swap
                    ramPage2RegisterValue = writeRegisterValue;
                    if (!chrRom8kbBankSwitchingMode) {
                        int numBanks = (rom.getCHRRomSize()) * ((chrRom8kbBankSwitchingMode) ? 1 : 2);
                        if (numBanks > 0 && ramPage2RegisterValue <= numBanks) {
                            //Ignore swap request if in 8kb swap mode
                            if (!chrRom8kbBankSwitchingMode) {
                                loadChrRomBank(0x1000, ramPage2RegisterValue);
                            }
                        }
                    }
                } else if (writeToMMC1RomPageRegister) {
                    //Initiate PRG Rom Bank Swap
                    romPageRegisterValue = writeRegisterValue;

                    bool ramEnabled = (romPageRegisterValue & 0b10000) == 0b10000;

                    //Supposedly we're supposed to ignore bit 0 in 32 kb mode
                    if (prgRomBankSwitchingMode < 2) {
                        romPageRegisterValue &= 0b00001110;
                    } else {
                        ramPage1RegisterValue &= 0b00001111;
                    }

                    if (prgRomBankSwitchingMode < 2) {
                        //32kb Prg Rom Banking
                        loadPrgRomBank(0x8000, romPageRegisterValue);
                    } else if (prgRomBankSwitchingMode == 2) {
                        loadPrgRomBank(0x8000, 0);
                        loadPrgRomBank(0xC000, romPageRegisterValue);
                    } else if (prgRomBankSwitchingMode == 3) {
                        loadPrgRomBank(0x8000, romPageRegisterValue);
                        loadPrgRomBank(0xC000, (unsigned char)(rom.getProgramRamSize() - 1));
                    }
                }

                //Reset shift register
                writeRegisterShift = 0;
                writeRegisterValue = 0;
            }
        }
    }
}

void MMC1::loadChrRomBank(ushort address, unsigned char bankNumber) {
    //0x0000-0x0FFF Chr Rom Data Bank 1
    //0x1000-0x1FFF Chr Rom Data Bank 2

    int bankSize = chrRom8kbBankSwitchingMode ? 0x2000 : 0x1000;
    uint bankAddress = (uint)(0x1000 * bankNumber);
    uint prgRomDataAddress = (uint)(0x4000 * rom.getProgramRomSize()); //Skip trainer if it exists

    for (int i = 0; i < bankSize; i++) {
        ushort writeAddress = (ushort)(address + i);
        uint readAddress = (uint)(prgRomDataAddress + bankAddress + i);
        if (writeAddress >= 0x0000 && writeAddress < (address + bankSize) && readAddress < (rom.getExactDataLength() - 16)) { //16 to skip the INES header.
            ppu.writePPURamByte(writeAddress, rom.readByte(16 + readAddress));
        }
    }
}

void MMC1::loadPrgRomBank(ushort address, unsigned char bankNumber) {
    //0x8000-0xBFFF, starts with first rom bank
    //0xC000-0xFFFF, starts with last rom bank

    int bankSize = (prgRomBankSwitchingMode < 2) ? 0x8000 : 0x4000;
    uint bankAddress = (uint)(0x4000 * (bankNumber));
    uint prgRomDataAddress = (uint)((rom.getTrainerIncluded()) ? 0x0200 : 0x0000); //Skip trainer if it exists

    for (int i = 0; i < bankSize; i++) {
        ushort writeAddress = (ushort)(address + i);
        uint readAddress = (uint)(prgRomDataAddress + bankAddress + i);

        if (writeAddress >= 0x8000 && writeAddress <= 0xFFFF && readAddress < (rom.getExactDataLength() - 16)) { //16 to skip the INES header.
            cpu.directCPURamWrite(writeAddress, rom.readByte(16 + readAddress));
        }
    }
}

void MMC1::resetMMC1() {
    writeRegisterShift = 0;
    writeRegisterValue = 0;

    controlRegisterValue = 0;
    ramPage1RegisterValue = 0;
    ramPage2RegisterValue = 0;
    romPageRegisterValue = 0;

    prgRomBankSwitchingMode = 3;
    chrRom8kbBankSwitchingMode = false;
}
