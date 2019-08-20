#ifndef MMC1_H
#define MMC1_H

#include <QObject>

class Rom;
class Ppu;
class Cpu;
class MMC1 : public QObject
{
    Q_OBJECT
public:
    explicit MMC1(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu);
    void loadRom();
    bool isMapperWriteAddress(ushort address);
    void writeMMC1(ushort address, unsigned char value);
    void loadChrRomBank(ushort address, unsigned char bankNumber);
    void loadPrgRomBank(ushort address, unsigned char bankNumber);

private:
    Rom& rom;
    Ppu& ppu;
    Cpu& cpu;

    unsigned char writeRegisterShift = 0;
    unsigned char writeRegisterValue = 0;

    unsigned char controlRegisterValue = 0;
    unsigned char ramPage1RegisterValue = 0;
    unsigned char ramPage2RegisterValue = 0;
    unsigned char romPageRegisterValue = 0;

    unsigned char prgRomBankSwitchingMode = 3;
    bool chrRom8kbBankSwitchingMode = false;

signals:

public slots:
};

#endif // MMC1_H
