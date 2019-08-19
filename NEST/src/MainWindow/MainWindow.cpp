#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "src/Canvas.h"
#include "src/Core.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    hLayout = new QHBoxLayout(ui->centralWidget);
    hLayout->setContentsMargins(0,0,0,0);
    hLayout->addWidget(canvas);

    QImage image = QImage(256, 240, QImage::Format_RGB32);
    image.fill(0xFF8080FF);
    canvas->drawImage(image);
}

MainWindow::~MainWindow()
{
    removeEventFilter(core->getInputPointer());

    QObject::disconnect(core->getPpuPointer(), &Ppu::drawImageToScreen,
                     canvas, &Canvas::drawImage);

    delete ui;
    delete canvas;
    delete core;
    delete hLayout;
}
