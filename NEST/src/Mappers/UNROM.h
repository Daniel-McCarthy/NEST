#ifndef UNROM_H
#define UNROM_H

#include <QObject>

class Rom;
class Ppu;
class Cpu;
class UNROM : public QObject
{
    Q_OBJECT
public:
    explicit UNROM(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu);
    void loadRom();
    bool isMapperWriteAddress(ushort address);
    void writeUNROM(ushort address, unsigned char value);
    void loadPrgRomBank(ushort address, unsigned char bankNumber);
    void loadChrRomBank();

private:
    Rom& rom;
    Ppu& ppu;
    Cpu& cpu;

signals:

public slots:
};

#endif // UNROM_H
