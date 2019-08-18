#-------------------------------------------------
#
# Project created by QtCreator 2019-08-16T21:59:42
#
#-------------------------------------------------

QT       += core gui gamepad

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NEST
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        src/main.cpp \
        src/MainWindow/MainWindow.cpp \
    src/Canvas.cpp \
    src/Core.cpp \
    src/Cpu.cpp \
    src/Input.cpp \
    src/Binary.cpp

HEADERS += \
        src/MainWindow/MainWindow.h \
    src/Canvas.h \
    src/Core.h \
    src/Cpu.h \
    src/Input.h \
    src/Binary.h

FORMS += \
        src/MainWindow/MainWindow.ui

RESOURCES += \
    resources.qrc
