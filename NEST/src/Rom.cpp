#include "src/Rom.h"

Rom::Rom(QObject *parent) : QObject(parent) {}
Rom::~Rom() {}

Rom::Rom(unsigned char* romData, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++) {
        romData[i] = romData[i];
	}
    readRomHeader();
}

void Rom::readRomHeader()
{
    if (checkForINESFormat()) {
        // Read INES.
        parseINESHeader();

        loadSuccessful = true;
    }
}

bool Rom::checkForINESFormat()
{
    // Checks first for file bytes for "NES" and MS-DOS eof signature.

    if (romData.length() >= 4) {
        return ((romData[0] << 24) | (romData[1] << 16) | (romData[2] << 8) | romData[3]) == 0x4E45531A;
    }

    return false;
}

bool Rom::checkForINES2Format()
{
    // Checks for NES 2.0 flags being set in byte 8 of 16.

    if (romData.length() >= 8) {
        // If bits 0000xx00 contain the value 2, then this header is NES 2.0 format.

        return ((romData[7] & 0x0C) == 2);
    }

    return false;
}

void Rom::parseINESHeader()
{
    if (romData.length() >= 16) {
        programRomSize = romData[4];
        chrRomSize = romData[5];

        uchar flags6 = romData[6];

        verticalMirroring = ((flags6 & 0x01) != 0);
        programRamBattery = ((flags6 & 0x02) != 0);
        trainerIncluded = ((flags6 & 0x04) != 0);
        ignoreMirroring = ((flags6 & 0x08) != 0);

        mapperSetting = (uchar)((flags6 & 0xF0) >> 4);

        uchar flags7 = romData[7];

        mapperSetting |= (uchar)((flags7 & 0xF0));

        programRamSize = romData[8];

        //TODO: Support other Flags 7 data.
        //TODO: Support flags 9 and 10.
    }
}

bool Rom::loadRom(QString path)
{
    QFile selectedFile(path);
    if (selectedFile.open(QIODevice::ReadOnly)) {
        QByteArray newRomData = selectedFile.readAll();

        if (newRomData.length() > 0)
        {
            romData = newRomData;
            romFilePath = path;
            return true;
        }
        else
        {
//            cout << "Error: Rom was unable to be opened or contains no data." << endl;
            selectedFile.close();
            return false;
        }
    }
    else
    {
//        cout << "Error: Rom does not exist." << endl;
        return false;
    }
}

uchar Rom::readByte(uint address)
{
    return romData[address];
}

QByteArray Rom::readBytes(ushort address, int byteCount)
{
    if ((address + (byteCount - 1)) < (getExactDataLength()))
    {
        QByteArray data = QByteArray(byteCount, 0);

        for(int i = 0; i < byteCount; i++)
        {
            data[i] = romData[address + i];
        }

        return data;
    }

    return NULL;
}

QByteArray Rom::readBytesFromAddressToEnd(ushort address)
{
    int dataLength = romData.length();

    if(address < dataLength)
    {
        QByteArray data = QByteArray(dataLength, 0);

        for(int i = 0; i < dataLength; i++)
        {
            data[i] = romData[address + i];
        }

        return data;
    }

    return NULL;
}

int Rom::getExactDataLength()
{
    return romData.length();
}

int Rom::getProgramRomSize()
{
    return programRomSize;
}

int Rom::getProgramRamSize()
{
    return programRamSize;
}

int Rom::getCHRRomSize()
{
    return chrRomSize;
}

int Rom::getMapperSetting()
{
    return mapperSetting;
}

bool Rom::getVerticalMirroring()
{
    return verticalMirroring;
}

bool Rom::getProgramRamBattery()
{
    return programRamBattery;
}

bool Rom::getTrainerIncluded()
{
    return trainerIncluded;
}

bool Rom::getIgnoreMirroring()
{
    return ignoreMirroring;
}

bool Rom::getUsesProgramRam()
{
    return usesProgramRam;
}

void Rom::resetRom() {
    programRomSize = 0;
    programRamSize = 0;
    chrRomSize = 0;
    mapperSetting = 0;

    verticalMirroring = false;
    programRamBattery = false;
    trainerIncluded = false;
    ignoreMirroring = false;
    usesProgramRam = false;

    romData = QByteArray();
    loadSuccessful = false;
    romFilePath = "";
}

bool Rom::usesSaveRam() {
    int mapperSetting = getMapperSetting();
    return mapperSetting == NROM_ID || mapperSetting == MMC1_ID || mapperSetting == MMC3_ID || mapperSetting == MMC5_ID;
}
