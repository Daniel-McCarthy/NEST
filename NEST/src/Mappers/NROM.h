#pragma once

#ifndef NROM_H
#define NROM_H

#include <QObject>

class Rom;
class Ppu;
class Cpu;
class NROM : public QObject
{
    Q_OBJECT
public:
    explicit NROM(QObject *parent, Rom& rom, Ppu& ppu, Cpu& cpu);
    void loadRom();
    void loadChrRom();
    void loadPrgRom();

private:
    Rom& rom;
    Ppu& ppu;
    Cpu& cpu;

signals:

public slots:
};

#endif // NROM_H
