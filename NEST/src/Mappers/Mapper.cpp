#include "Mapper.h"
#include "src/Rom.h"
#include "src/Mappers/MMC1.h"
#include "src/Mappers/UNROM.h"

Mapper::Mapper(QObject *parent, Rom& rom, MMC1& mmc1, UNROM& unrom) : QObject(parent), rom(rom), mmc1(mmc1), unrom(unrom)
{
}

bool Mapper::isMapperWriteAddress(ushort address) {
    int mapperSetting = rom.getMapperSetting();
    if (mapperSetting == 1) {
        return mmc1.isMapperWriteAddress(address);
    } else if (mapperSetting == 2) {
        return unrom.isMapperWriteAddress(address);
    }

    return false;
}

void Mapper::writeToCurrentMapper(ushort address, unsigned char value) {
    int mapperSetting = rom.getMapperSetting();

    if (mapperSetting == 1) {
        mmc1.writeMMC1(address, value);
    } else if (mapperSetting == 2) {
        unrom.writeUNROM(address, value);
    }
}
