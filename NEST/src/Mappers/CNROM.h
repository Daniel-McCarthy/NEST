#ifndef CNROM_H
#define CNROM_H

#include <QObject>

class Rom;
class Ppu;
class Cpu;
class CNROM : public QObject
{
    Q_OBJECT
public:
    explicit CNROM(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu);
    void loadRom();
    bool isMapperWriteAddress(ushort address);
    void writeCNROM(ushort address, unsigned char value);
    void loadChrRomBank(ushort address, unsigned char bankNumber);
    void loadPrgRomBank(ushort address);

private:
    Rom& rom;
    Ppu& ppu;
    Cpu& cpu;

signals:

public slots:
};

#endif // CNROM_H
