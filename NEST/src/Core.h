#pragma once

#ifndef CORE_H
#define CORE_H

#include <QWidget>
#include "src/Rom.h"
#include "src/Cpu.h"
#include "src/Input.h"
#include "src/Ppu.h"
#include "src/Mappers/Mapper.h"
#include "src/Mappers/NROM.h"
#include "src/Mappers/MMC1.h"
#include "src/Mappers/UNROM.h"
#include "src/Mappers/CNROM.h"
#include "src/Mappers/MMC3.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QWidget *parent = 0);
    virtual ~Core();
    Rom* getRomPointer();
    Cpu* getCPUPointer();
    Input* getInputPointer();
    Ppu* getPpuPointer();
    Mapper* getMapperPointer();
    NROM* getNROMPointer();
    MMC1* getMMC1Pointer();
    UNROM* getUNROMPointer();
    CNROM* getCNROMPointer();
    MMC3* getMMC3Pointer();

    bool run = false;
    bool paused = false;
    bool step = false;

    uint TOTAL_CPU_CLOCKS = 0;
    uint TOTAL_PPU_CLOCKS = 0;

    void emulationLoop();
    void resetCore();

private:
    Rom rom;
    Cpu cpu;
    Ppu ppu;
    Input input;
    Mapper mapper;
    NROM nrom;
    MMC1 mmc1;
    UNROM unrom;
    CNROM cnrom;
    MMC3 mmc3;

    const int NROM_ID   = 0x00;
    const int MMC1_ID   = 0x01;
    const int UxROM_ID  = 0x02;
    const int CNROM_ID  = 0x03;
    const int MMC3_ID   = 0x04;
    const int MMC5_ID   = 0x05;

signals:

public slots:
    void setPaused(bool setting);
    void setRun(bool setting);
    void setStep(bool setting);
};

#endif // CORE_H
