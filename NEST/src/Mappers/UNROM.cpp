#include "UNROM.h"
#include "src/Rom.h"
#include "src/Ppu.h"
#include "src/Cpu.h"

UNROM::UNROM(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu) : QObject(parent), rom(rom), ppu(ppu), cpu(cpu)
{
}

void UNROM::loadRom() {
    if (rom.getMapperSetting() == 2) {
        loadPrgRomBank(0x8000, 0);
        loadPrgRomBank(0xC000, (unsigned char)(rom.getProgramRomSize() - 1));

        if (rom.getCHRRomSize() > 0) {
            loadChrRomBank();
        }

        ppu.isNametableMirrored = true;
        ppu.isHorizNametableMirror = (!rom.getVerticalMirroring());
        ppu.isVertNametableMirror = (rom.getVerticalMirroring());
    }
}

bool UNROM::isMapperWriteAddress(ushort address) {
    return address >= 0x8000 && address <= 0xFFFF;
}

void UNROM::writeUNROM(ushort address, unsigned char value) {
    bool writeToUNROMRomPageRegister = address >= 0x8000 && address <= 0xFFFF;

    if (writeToUNROMRomPageRegister) {
        unsigned char bankNumber = (unsigned char)(value & 0x0F);
        loadPrgRomBank(0x8000, bankNumber);
    }
}

void UNROM::loadPrgRomBank(ushort address, unsigned char bankNumber) {
    //0x8000-0xBFFF, starts with first rom bank
    //0xC000-0xFFFF, fixed to last rom bank

    uint bankAddress = (uint)(0x4000 * (bankNumber));
    uint prgRomDataAddress = (uint)((rom.getTrainerIncluded()) ? 0x0200 : 0x0000); //Skip trainer if it exists

    for (int i = 0; i < 0x4000; i++)  {
        ushort writeAddress = (ushort)(address + i);
        uint readAddress = (uint)(prgRomDataAddress + bankAddress + i);
        if (writeAddress >= 0x8000 && writeAddress <= 0xFFFF && readAddress < (rom.getExactDataLength() - 16)) { //16 in order to skip the INES header
            cpu.directCPURamWrite(writeAddress, rom.readByte(readAddress + 16));
        }
    }
}

void UNROM::loadChrRomBank() {
    //0x0000-0x0FFF Chr Rom Data Bank 1
    //0x1000-0x1FFF Chr Rom Data Bank 2

    uint chrDataAddress = (uint)(0x2000 * rom.getProgramRomSize());

    for (uint i = 0; i < 0x2000; i++) {
        if ((chrDataAddress + i) < (rom.getExactDataLength() - 16)) { //16 in order to skip the INES header
            ppu.writePPURamByte((ushort)i, rom.readByte(chrDataAddress + i + 16));
        }
    }
}
