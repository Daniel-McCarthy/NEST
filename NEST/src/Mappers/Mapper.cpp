#include "Mapper.h"
#include "src/Rom.h"
#include "src/Mappers/MMC1.h"
#include "src/Mappers/UNROM.h"
#include "src/Mappers/CNROM.h"

Mapper::Mapper(QObject *parent, Rom& rom, MMC1& mmc1, UNROM& unrom, CNROM& cnrom) : QObject(parent), rom(rom), mmc1(mmc1), unrom(unrom), cnrom(cnrom)
{
}

bool Mapper::isMapperWriteAddress(ushort address) {
    int mapperSetting = rom.getMapperSetting();
    if (mapperSetting == 1) {
        return mmc1.isMapperWriteAddress(address);
    } else if (mapperSetting == 2) {
        return unrom.isMapperWriteAddress(address);
    } else if (mapperSetting == 3) {
        return cnrom.isMapperWriteAddress(address);
    }

    return false;
}

void Mapper::writeToCurrentMapper(ushort address, unsigned char value) {
    int mapperSetting = rom.getMapperSetting();

    if (mapperSetting == 1) {
        mmc1.writeMMC1(address, value);
    } else if (mapperSetting == 2) {
        unrom.writeUNROM(address, value);
    } else if (mapperSetting == 3) {
        cnrom.writeCNROM(address, value);
    }
}
