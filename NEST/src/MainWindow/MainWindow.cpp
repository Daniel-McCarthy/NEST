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
    QString initialDirectory = QDir::homePath();
    if (initialDirectory == "" ) {
        initialDirectory = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    }

    QString filePath = QFileDialog::getOpenFileName(this, ("Open NES File"),
                                                      initialDirectory,
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

    if (mapperSetting == 0) {
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

    QString romPath = core->getRomPointer()->romFilePath;
    QString savePath = romPath.left(romPath.lastIndexOf('.')) + ".sav";
    loadSaveFile(savePath);

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

void MainWindow::on_actionSetScale1x_triggered() {
    setWindowScale(1);
}

void MainWindow::on_actionSetScale2x_triggered() {
    setWindowScale(2);
}

void MainWindow::on_actionSetScale3x_triggered() {
    setWindowScale(3);
}

void MainWindow::on_actionSetScale4x_triggered() {
    setWindowScale(4);
}

void MainWindow::setWindowScale(uint multiple) {
    uint nesWidth = 256;
    uint nesHeight = 240;
    uint scaledWidth = nesWidth * multiple;
    uint scaledHeight = nesHeight * multiple;
    QDesktopWidget desktopWidget;
    QRect monitorRect = desktopWidget.screenGeometry(desktopWidget.primaryScreen());

    if (scaledHeight > monitorRect.height() || scaledWidth > monitorRect.width()) {
        uint newMultiple = monitorRect.height() / nesHeight;
        scaledWidth = nesWidth * newMultiple;
        scaledHeight = nesHeight * newMultiple;
    }

    uint newWindowWidth = scaledWidth;
    uint newWindowHeight = scaledHeight + ui->menuBar->height();

    resize(newWindowWidth, newWindowHeight);
}

bool MainWindow::loadSaveFile(QString path) {
    //Attempt to open file, if it was successful close it and call the full loadSave function
    QString savePath = path.left(path.indexOf('.')) + ".sav";
    QFile saveFile(savePath);

    bool fileOpen = saveFile.open(QIODevice::ReadOnly);

    bool saveFileLoaded = false;

    if (fileOpen) {
        QByteArray binaryFileData = saveFile.readAll();
        saveFileLoaded = loadSaveFile(binaryFileData);
    }
    saveFile.close();
    return saveFileLoaded;
}

/*
Load Save File From Array
*/
bool MainWindow::loadSaveFile(QByteArray saveFile) {
    Cpu* cpu = core->getCPUPointer();
    int fileLength = saveFile.length();
    bool fileNotEmpty = fileLength > 0;
    int numBytesToRead = (fileLength >= 0x1FFF) ? 0x1FFF : fileLength;

    if (fileNotEmpty) {
        unsigned short address = 0x6000;
        for (unsigned short i = 0x0; i <= numBytesToRead; i++) {
            cpu->writeCPURam((ushort)(address + i), (unsigned char)saveFile[i], true);
        }
        return true;
    } else {
        //cout << "Error: Save file has no data." << endl;
        return false;
    }
}

QByteArray MainWindow::returnSaveDataFromCpuRam() {
    QByteArray memory;
    Cpu* cpu = core->getCPUPointer();
    unsigned short address = 0x6000;
    for (int i = 0; i <= 0x1FFF; i++) {
        memory.push_back(cpu->readCPURam((ushort)(address + i), true));
    }

    return memory;
}

bool MainWindow::createSaveFile(bool overwrite) {
    Rom* rom = core->getRomPointer();
    unsigned char mapperSetting = rom->getMapperSetting();
    bool romUsesRam = mapperSetting == rom->NROM_ID || mapperSetting == rom->MMC1_ID || mapperSetting == rom->MMC3_ID || mapperSetting == rom->MMC5_ID;

    if (romUsesRam) {
        QByteArray saveData = returnSaveDataFromMemory();

        //Attempt to open file, to check if it exists and potentially overwrite it.
        QString savePath = rom->romFilePath.left(rom->romFilePath.lastIndexOf('.')) + ".sav";
        QFile saveFile(savePath);

        bool fileExists = saveFile.open(QIODevice::ReadWrite);

        if (!fileExists || overwrite) {
            saveFile.write(saveData);
            return true;
        }
    }

    return false;
}
