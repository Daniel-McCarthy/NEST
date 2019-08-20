#include "Mapper.h"
#include "src/Rom.h"


Mapper::Mapper(QObject *parent, Rom& rom) : QObject(parent), rom(rom)
{
}

bool Mapper::isMapperWriteAddress(ushort address) {

    return false;
}

void Mapper::writeToCurrentMapper(ushort address, unsigned char value) {
    int mapperSetting = rom.getMapperSetting();
}
