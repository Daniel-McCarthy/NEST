#include "Core.h"
#include <QCoreApplication>

Core::Core(QWidget *parent)
    : QObject(parent),
        rom(this),
        cpu(input),
        input(this),
        ppu(nullptr, cpu)
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
            uint cpuClocks = cpu.mClock;
            uint ppuClocks = cpuClocks * 3;

            TOTAL_CPU_CLOCKS += cpuClocks;
            TOTAL_PPU_CLOCKS += ppuClocks;

            step = false;

            cpu.mClock = 0;
            cpu.tClock = 0;
        }
    }
}
