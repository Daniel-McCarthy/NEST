#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "src/Canvas.h"
#include "src/Core.h"
#include "src/ColorDialog/ColorDialog.h"

#include "QFileDialog"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionPause->setEnabled(false);
    ui->actionResume->setEnabled(false);
    ui->actionResume->setVisible(false);

    core = new Core();
    canvas = new Canvas(this);
    canvas->move(0,21);
    canvas->resize(256, 240);
    canvas->stackUnder(ui->menuBar);
    ui->menuBar->raise();


    setWindowIcon(QIcon(":/Nest.ico"));

    installEventFilter(core->getInputPointer());

    QObject::connect(core->getPpuPointer(), &Ppu::drawImageToScreen,
                     canvas, &Canvas::drawImage);

    QObject::connect(this, &MainWindow::setEmulationPaused,
                             core, &Core::setPaused);

    QObject::connect(this, &MainWindow::setEmulationRun,
                             core, &Core::setRun);

    hLayout = new QHBoxLayout(ui->centralWidget);
    hLayout->setContentsMargins(0,0,0,0);
    hLayout->addWidget(canvas);

    QImage image = QImage(256, 240, QImage::Format_RGB32);
    image.fill(0xFF8080FF);
    canvas->drawImage(image);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    setEmulationRun(false);
    event->accept();
}

MainWindow::~MainWindow()
{
    removeEventFilter(core->getInputPointer());

    QObject::disconnect(core->getPpuPointer(), &Ppu::drawImageToScreen,
                     canvas, &Canvas::drawImage);

    QObject::disconnect(this, &MainWindow::setEmulationPaused,
                             core, &Core::setPaused);

    QObject::disconnect(this, &MainWindow::setEmulationRun,
                             core, &Core::setRun);

    delete ui;
    delete canvas;
    delete core;
    delete hLayout;
}

void MainWindow::on_actionOpen_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, ("Open NES File"),
                                                      "/home",
                                                      ("NES Files (*.nes)"));
    if (filePath == "") {
        return;
    }

    setEmulationPaused(true);
    setEmulationRun(false);

    Rom* rom = core->getRomPointer();
    bool loadedSuccessfully = rom->loadRom(filePath);

    if (loadedSuccessfully == false) {
        return;
    }

    ui->actionPause->setEnabled(true);
    ui->actionResume->setEnabled(true);

    rom->readRomHeader();
    int mapperSetting = rom->getMapperSetting();

    if (mapperSetting == 0)
    {
        core->getNROMPointer()->loadRom();
    } else if (mapperSetting == 1) {
        core->getMMC1Pointer()->loadRom();
    } else if (mapperSetting == 2) {
        core->getUNROMPointer()->loadRom();
    } else if (mapperSetting == 3) {
        core->getCNROMPointer()->loadRom();
    } else if (mapperSetting == 4) {
        core->getMMC3Pointer()->loadRom();
    }

    ushort resetAddress = 0;
    Cpu* cpu = core->getCPUPointer();
    resetAddress |= cpu->readCPURam(0xFFFC, true);
    resetAddress |= (ushort)(cpu->readCPURam(0xFFFD, true) << 8);
    cpu->programCounter = resetAddress;

    setEmulationPaused(false);
    setEmulationRun(true);
    startEmulationThread();

}

void MainWindow::startEmulationThread() {
    core->emulationLoop();
}

void MainWindow::endEmulationThread() {
    emit setEmulationRun(false);
}


void MainWindow::on_actionPause_triggered() {
    ui->actionPause->setVisible(false);
    ui->actionResume->setVisible(true);
    setEmulationPaused(true);
}

void MainWindow::on_actionResume_triggered() {
    ui->actionPause->setVisible(true);
    ui->actionResume->setVisible(false);
    setEmulationPaused(false);
}

void MainWindow::on_actionPalette_triggered() {
    if (currentColorDialog != NULL) {
        return;
    }

    currentColorDialog = new ColorDialog(this, core->getPpuPointer());

    QObject::connect(currentColorDialog, &ColorDialog::finished,
                     this, &MainWindow::paletteWindowClosed);

    currentColorDialog->show();
}

void MainWindow::paletteWindowClosed() {
    QObject::disconnect(currentColorDialog, &ColorDialog::finished,
                        this, &MainWindow::paletteWindowClosed);

    delete currentColorDialog;
    currentColorDialog = NULL;
}

