#include "Core.h"
#include <QCoreApplication>

Core::Core(QWidget *parent)
    : QObject(parent),
        cpu()
{
}

Core::~Core() {

Cpu* Core::getCPUPointer() {
    return &cpu;
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
    while(Core.run) {
        if (!paused || step) {

            step = false;

            cpu.mClock = 0;
            cpu.tClock = 0;

        if (screen.processPolling) {
            //Poll QT Events
        }
    }
}
