#include "NROM.h"
#include "src/Rom.h"
#include "src/Ppu.h"
#include "src/Cpu.h"

NROM::NROM(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu) : QObject(parent), rom(rom), ppu(ppu), cpu(cpu)
{
}

void NROM::loadRom() {
    if (rom.getMapperSetting() == 0) {
        loadPrgRom();
        loadChrRom();

        ppu.isNametableMirrored = true;
        ppu.isHorizNametableMirror = (!rom.getVerticalMirroring());
        ppu.isVertNametableMirror = (rom.getVerticalMirroring());

    }
}

void NROM::loadChrRom() {
    uint chrDataAddress = (uint)(0x4000 * rom.getProgramRomSize());
    if (rom.getCHRRomSize() != 0) {
        for (uint i = 0; i < 0x2000; i++) {
            if ((chrDataAddress + i) < (rom.getExactDataLength() - 16)) { //16 in order to skip the INES header
                ppu.writePPURamByte((ushort)i, rom.readByte(chrDataAddress + i + 16));
            }
        }
    }
}

void NROM::loadPrgRom() {

    int programSize = rom.getProgramRomSize() * 0x8000;

    for (uint i = 0; i < programSize; i++) {
        ushort address = (ushort)(0x8000 + i);
        if (address >= 0x8000 && address <= 0xFFFF && i < (rom.getExactDataLength() - 16)) { //16 in order to skip the INES header
            cpu.directCPURamWrite(address, rom.readByte(i + 16));
        }
    }

    // If there is only one bank, then mirror
    if(rom.getProgramRomSize() == 1) {
        for (uint i = 0; i < 0x4000; i++) {
            ushort address = (ushort)(0xC000 + i);
            if (address >= 0xC000 && address <= 0xFFFF && i < (rom.getExactDataLength() - 16)) { //16 in order to skip the INES header
                cpu.directCPURamWrite(address, rom.readByte(i + 16));
            }
        }
    }
}
