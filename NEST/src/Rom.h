#include <QString>
#include <QObject>
#include <QFile>

#ifndef ROMH
#define ROMH


#pragma once
class Memory;
class Rom : public QObject
{
    Q_OBJECT

private:
    int programRomSize = 0; // x * 16 KB data size.
    int programRamSize = 0; // x * 8  KB data size. // If this value is zero, it should be assumed to be 8 KB.
    int chrRomSize = 0; // X * 8  KB data size. // If this value is zero, then CHR Ram is to be used.
    uchar mapperSetting = 0;

    bool verticalMirroring = false;
    bool programRamBattery = false;
    bool trainerIncluded = false;
    bool ignoreMirroring = false; // True infers use of 4 screen vram.
    bool usesProgramRam = false; // INES 2.0 feature.

public:
    QByteArray romData;
    bool loadSuccessful = false;
    QString romFilePath = "";

    Rom(QObject *parent = nullptr);
    Rom(unsigned char* romData, unsigned int length);
	~Rom();
	void readRomHeader();
    bool checkForINESFormat();
    bool checkForINES2Format();
    void parseINESHeader();
    bool loadRom(QString path);
    int getExactDataLength();
    int getProgramRomSize();
    int getProgramRamSize();
    int getCHRRomSize();
    int getMapperSetting();
    bool getVerticalMirroring();
    bool getProgramRamBattery();
    bool getTrainerIncluded();
    bool getIgnoreMirroring();
    bool getUsesProgramRam();

	unsigned char readByte(unsigned int address);
    QByteArray readBytes(ushort address, int byteCount);
    QByteArray readBytesFromAddressToEnd(ushort address);

    signals:
};
#endif
