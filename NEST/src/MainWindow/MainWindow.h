#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QImage>

namespace Ui {
class MainWindow;
}

class Core;
class Canvas;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Core* core = nullptr;
    Canvas* canvas;
    QHBoxLayout* hLayout;

    void startEmulationThread();
    void endEmulationThread();

    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionOpen_triggered();
    void on_actionPause_triggered();
    void on_actionResume_triggered();

signals:
    void onGameFileOpened(QString filePath);
    void setEmulationPaused(bool setting);
    void setEmulationRun(bool setting);

};

#endif // MAINWINDOW_H
