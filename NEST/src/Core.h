#ifndef CORE_H
#define CORE_H

#include <QWidget>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QWidget *parent = 0);
    virtual ~Core();

    bool run = false;
    bool paused = false;
    bool step = false;
    bool pendingNMI = false;

    uint TOTAL_CPU_CLOCKS = 0;
    uint TOTAL_PPU_CLOCKS = 0;

    void emulationLoop();

private:

signals:

public slots:
    void setPaused(bool setting);
    void setRun(bool setting);
    void setStep(bool setting);
};

#endif // CORE_H
