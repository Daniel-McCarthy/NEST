#include "Core.h"
#include <QCoreApplication>

Core::Core(QWidget *parent)
    : QObject(parent)
{
}

Core::~Core() {
}

void Core::emulationLoop() {
    while(Core.run) {
        if (!paused || step) {

            step = false;

            cpu.mClock = 0;
            cpu.tClock = 0;
        }

        if (screen.processPolling) {
            //Poll QT Events
            QCoreApplication::processEvents();
            screen.processPolling = false;
        }
    }
}
