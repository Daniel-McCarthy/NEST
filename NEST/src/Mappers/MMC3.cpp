#include "MMC3.h"
#include "src/Rom.h"
#include "src/Ppu.h"
#include "src/Cpu.h"

MMC3::MMC3(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu) : QObject(parent), rom(rom), ppu(ppu), cpu(cpu)
{
}

void MMC3::loadRom() {
    if (rom.getMapperSetting() == 4) {
        loadChrRomBank(0x0000, 0x0800, 0);
        loadChrRomBank(0x0800, 0x0800, 0);
        loadChrRomBank(0x1000, 0x0800, 0);
        loadChrRomBank(0x1800, 0x0800, 0);

        loadPrgRomBank(0x8000, 0);
        loadPrgRomBank(0xA000, 0);
        loadPrgRomBank(0xC000, (unsigned char)((rom.getProgramRomSize() * 2) - 2));
        loadPrgRomBank(0xE000, (unsigned char)((rom.getProgramRomSize() * 2) - 1));

        ppu.isNametableMirrored = false;
    }
}

bool MMC3::isMapperWriteAddress(ushort address) {
    return address >= 0x8000 && address <= 0xFFFF;
}

void MMC3::writeMMC3(ushort address, unsigned char value) {
    bool isAddressEven = (address % 2) == 0;
    bool bankSelect = (address >= 0x8000 && address <= 0x9FFF) && isAddressEven;
    bool bankSwapNumber = (address >= 0x8000 && address <= 0x9FFF) && !isAddressEven;
    bool mirroringSelection = (address >= 0xA000 && address <= 0xBFFF) && isAddressEven;
    bool ramSetting = (address >= 0xA000 && address <= 0xBFFF) && !isAddressEven;
    bool irqLatchSetting = (address >= 0xC000 && address <= 0xDFFF) && isAddressEven;
    bool irqReset = (address >= 0xC000 && address <= 0xDFFF) && !isAddressEven;
    bool irqDisable = (address >= 0xE000 && address <= 0xFFFF) && isAddressEven;
    bool irqEnable = (address >= 0xE000 && address <= 0xFFFF) && !isAddressEven;

    if (address >= 0x8000 && address <= 0xFFFF) {
        if (bankSelect) {
            bankRegister = (unsigned char)(value & 0b111);
            prgRomBankingMode = (value & 0b01000000) != 0;
            chrRomBankOrder = (value & 0b10000000) != 0;
        } else if (bankSwapNumber) {
            unsigned char bankNumber = value;

            if (bankRegister == 0) {
                //Swap 0x0000-0x07FF / 0x1000-0x17FF 2kb CHR Rom Bank
                ushort ramAddress = (ushort)(chrRomBankOrder ? 0x1000 : 0x0000);
                ushort bankSize = 0x0800;

                loadChrRomBank(ramAddress, bankSize, bankNumber);
            } else if (bankRegister == 1) {
                //Swap 0x0800-0x0FFF / 0x1800-0x1FFF 2kb CHR Rom Bank
                ushort ramAddress = (ushort)(chrRomBankOrder ? 0x1800 : 0x0800);
                ushort bankSize = 0x0800;

                loadChrRomBank(ramAddress, bankSize, bankNumber);
            } else if (bankRegister == 2) {
                //Swap 0x1000-0x13FF / 0x0000-0x03FF 1kb CHR Rom Bank
                ushort ramAddress = (ushort)(chrRomBankOrder ? 0x0000 : 0x1000);
                ushort bankSize = 0x0400;

                loadChrRomBank(ramAddress, bankSize, bankNumber);
            } else if (bankRegister == 3) {
                //Swap 0x1400-0x17FF / 0x0400-0x07FF 1kb CHR Rom Bank
                ushort ramAddress = (ushort)(chrRomBankOrder ? 0x0400 : 0x1400);
                ushort bankSize = 0x0400;

                loadChrRomBank(ramAddress, bankSize, bankNumber);
            } else if (bankRegister == 4) {
                //Swap 0x1800-0x1BFF / 0x0800-0x0BFF 1kb CHR Rom Bank
                ushort ramAddress = (ushort)(chrRomBankOrder ? 0x0800 : 0x1800);
                ushort bankSize = 0x0400;

                loadChrRomBank(ramAddress, bankSize, bankNumber);
            } else if (bankRegister == 5) {
                //Swap 0x1C00-0x1FFF / 0x0C00-0x0FFF 1kb CHR Rom Bank
                ushort ramAddress = (ushort)(chrRomBankOrder ? 0x0C00 : 0x1C00);
                ushort bankSize = 0x0400;

                loadChrRomBank(ramAddress, bankSize, bankNumber);
            } else if (bankRegister == 6) {
                //Swap 0x8000-0x9FFF / 0xC000-0xDFFF 8kb PRG Rom Bank
                ushort ramAddress = (ushort)(prgRomBankingMode ? 0xC000 : 0x8000);
                loadPrgRomBank(ramAddress, (unsigned char)(bankNumber));

                //Fix bank to second to last rom bank
                ushort fixedBankRamAddress = (ushort)(prgRomBankingMode ? 0x8000 : 0xC000);
                loadPrgRomBank(fixedBankRamAddress, (unsigned char)((rom.getProgramRomSize() * 2) - 2));
            } else if (bankRegister == 7) {
                //Swap 0xA000-0xBFFF 8kb PRG Rom Bank
                loadPrgRomBank(0xA000, bankNumber);
            }
        } else if (mirroringSelection) {
            ppu.isNametableMirrored = true;
            if ((value & 0x01) != 0) {
                //Set Nametable Mirroring to horizontal
                ppu.isHorizNametableMirror = true;
                ppu.isVertNametableMirror = false;
            } else {
                //Set Nametable Mirroring to vertical
                ppu.isHorizNametableMirror = false;
                ppu.isVertNametableMirror = true;
            }
        } else if (ramSetting) {
            bool ramWritesDisabled = (value & 0b01000000) != 0;
            bool ramEnabled = (value & 0b10000000) != 0;
            //TODO: Implement these settings
        } else if (irqLatchSetting) {
            irqLatch = value;
        } else if (irqReset) {
            irqCounter = irqLatch;
        } else if (irqDisable) {
            irqEnabled = false;
            //TODO: Register pending interrupts
        } else if (irqEnable) {
            irqEnabled = true;
        }
    }
}

void MMC3::loadChrRomBank(ushort address, ushort bankSize, unsigned char bankNumber) {
    //0x0000-0x07FF Chr Rom, 2k switchable bank
    //0x0800-0x0FFF Chr Rom, 2k switchable bank
    //0x1000-0x13FF Chr Rom, 1k switchable bank
    //0x1400-0x17FF Chr Rom, 1k switchable bank
    //0x1800-0x1BFF Chr Rom, 1k switchable bank
    //0x1C00-0x1FFF Chr Rom, 1k switchable bank

    //Order of 1k and 2k rom banks are switchable

    uint bankAddress = (uint)(0x0400 * bankNumber);
    uint prgRomDataAddress = (uint)(0x2000 * (rom.getProgramRomSize() * 2)); //Skip trainer if it exists

    for (int i = 0; i < bankSize; i++) {
        ushort writeAddress = (ushort)(address + i);
        uint readAddress = (uint)(prgRomDataAddress + bankAddress + i);
        if (writeAddress >= 0x0000 && writeAddress < (address + bankSize) && readAddress < (rom.getExactDataLength() - 16)) { //16 to skip the INES header.
            ppu.writePPURamByte(writeAddress, rom.readByte(16 + readAddress));
        }
    }
}

void MMC3::loadPrgRomBank(ushort address, unsigned char bankNumber) {
    //0x8000-0x9FFF, switchable rom bank
    //0xA000-0xBFFF, switchable rom bank

    //0xC000-0xDFFF, fixed to second to last rom bank
    //0xE000-0xFFFF, fixed to last rom bank

    //The order of the swappable and fixed banks can be switched.


    uint bankSize = 0x2000;
    uint romSize = (uint)((rom.getProgramRomSize() * 2) * 0x2000);
    uint bankAddress = (uint)(bankSize * (bankNumber));
    bankAddress %= romSize;
    uint prgRomDataAddress = (uint)((rom.getTrainerIncluded()) ? 0x0200 : 0x0000); //Skip trainer if it exists

    for (int i = 0; i < bankSize; i++) {
        ushort writeAddress = (ushort)(address + i);
        uint readAddress = (uint)(prgRomDataAddress + bankAddress + i);

        if (writeAddress >= 0x8000 && writeAddress <= 0xFFFF && readAddress < (rom.getExactDataLength() - 16)) { //16 to skip the INES header.
            cpu.directCPURamWrite(writeAddress, rom.readByte(16 + readAddress));
        }
    }
}

void MMC3::resetMMC3() {
    bankRegister = 0;
    prgRomBankingMode = false;
    chrRomBankOrder = false;

    irqLatch = 0;
    irqCounter = 0;
    irqEnabled = false;
    irqPending = false;
}
