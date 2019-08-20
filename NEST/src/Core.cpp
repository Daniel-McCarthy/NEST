#include "Core.h"
#include <QCoreApplication>

Core::Core(QWidget *parent)
    : QObject(parent),
        rom(this),
        cpu(input, ppu, mapper),
        input(this),
        ppu(nullptr, cpu, TOTAL_PPU_CLOCKS),
        mapper(nullptr, rom, mmc1),
        nrom(this, rom, ppu, cpu),
        mmc1(this, rom, ppu, cpu)
{
}

Core::~Core() {
}

Rom* Core::getRomPointer() {
    return &rom;
}

Cpu* Core::getCPUPointer() {
    return &cpu;
}

Input* Core::getInputPointer() {
    return &input;
}

Ppu* Core::getPpuPointer() {
    return &ppu;
}

Mapper* Core::getMapperPointer() {
    return &mapper;
}

NROM* Core::getNROMPointer() {
    return &nrom;
}

MMC1* Core::getMMC1Pointer() {
    return &mmc1;
}

void Core::setPaused(bool setting) {
    paused = setting;
}

void Core::setRun(bool setting) {
    run = setting;
}

void Core::setStep(bool setting) {
    step = setting;
}
void Core::emulationLoop() {
    while(run) {
        if (!paused || step) {
            if ( ppu.pendingNMI ) { cpu.serviceNonMaskableInterrupt(); }
            else { cpu.serviceInterrupt(); }

            cpu.fetchAndExecute();

            uint cpuClocks = cpu.mClock;
            uint ppuClocks = cpuClocks * 3;

            TOTAL_CPU_CLOCKS += cpuClocks;
            TOTAL_PPU_CLOCKS += ppuClocks;

            ppu.updatePPU(TOTAL_PPU_CLOCKS);

            step = false;

            cpu.mClock = 0;
            cpu.tClock = 0;
        }

        if (ppu.processPolling) {
            //Poll QT Events
            QCoreApplication::processEvents();
            ppu.processPolling = false;
        }
    }
}
