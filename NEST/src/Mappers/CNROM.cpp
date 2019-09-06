#include "CNROM.h"
#include "src/Rom.h"
#include "src/Ppu.h"
#include "src/Cpu.h"

CNROM::CNROM(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu) : QObject(parent), rom(rom), ppu(ppu), cpu(cpu)
{
}

void CNROM::loadRom() {
    if (rom.getMapperSetting() == 3) {
        bool onlyOneBank = rom.getProgramRomSize() == 1;
        loadPrgRomBank(0x8000);
        if (onlyOneBank) {
            loadPrgRomBank(0xC000);
        }
        loadChrRomBank(0x0000, 0);

        ppu.isNametableMirrored = true;
        ppu.isHorizNametableMirror = (!rom.getVerticalMirroring());
    }
}

bool CNROM::isMapperWriteAddress(ushort address) {
    return address >= 0x8000 && address <= 0xFFFF;
}

void CNROM::writeCNROM(ushort address, unsigned char value) {
    if (address >= 0x8000 && address <= 0xFFFF) {
        //Select CHR Rom Bank to swap to
        loadChrRomBank(0x0000, (unsigned char)(value & 0b11));
    }
}

void CNROM::loadChrRomBank(ushort address, unsigned char bankNumber) {
    //0x0000-0x1FFF Chr Rom Data Bank

    uint bankAddress = (uint)(0x2000 * (bankNumber));
    uint prgRomDataAddress = (uint)(0x4000 * rom.getProgramRomSize()); //Skip trainer if it exists

    for (int i = 0; i < 0x2000; i++) {
        ushort writeAddress = (ushort)(address + i);
        uint readAddress = (uint)(prgRomDataAddress + bankAddress + i);
        if (writeAddress >= 0x0000 && writeAddress <= 0x1FFF && readAddress < (rom.getExactDataLength() - 16)) { //16 in order to skip the INES header
            ppu.writePPURamByte(writeAddress, rom.readByte(readAddress + 16));
        }
    }
}

void CNROM::loadPrgRomBank(ushort address) {
    //0x8000 16kb - 32 kb non-switchable program rom

    uint prgRomDataAddress = (uint)((rom.getTrainerIncluded()) ? 0x0200 : 0x0000); //Skip trainer if it exists
    uint prgRomSize = (uint)(rom.getProgramRomSize() * 0x4000);

    for (int i = 0; i < prgRomSize; i++) {
        ushort writeAddress = (ushort)(address + i);
        uint readAddress = (uint)(prgRomDataAddress + i);
        if (writeAddress >= 0x8000 && writeAddress <= 0xFFFF && readAddress < (rom.getExactDataLength() - 16)) { //16 in order to skip the INES header
            cpu.directCPURamWrite(writeAddress, rom.readByte(readAddress + 16));
        }
    }
}
