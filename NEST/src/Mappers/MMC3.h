#ifndef MMC3_H
#define MMC3_H

#include <QObject>

class Rom;
class Ppu;
class Cpu;
class MMC3 : public QObject
{
    Q_OBJECT
public:
    explicit MMC3(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu);
    void loadRom();
    bool isMapperWriteAddress(ushort address);
    void writeMMC3(ushort address, unsigned char value);
    void loadChrRomBank(ushort address, ushort bankSize, unsigned char bankNumber);
    void loadPrgRomBank(ushort address, unsigned char bankNumber);
    void resetMMC3();

private:
    Rom& rom;
    Ppu& ppu;
    Cpu& cpu;

    unsigned char bankRegister;
    bool prgRomBankingMode;
    bool chrRomBankOrder;

    unsigned char irqLatch;
    unsigned char irqCounter;
    bool irqEnabled;
    bool irqPending;

signals:

public slots:
};

#endif // MMC3_H
