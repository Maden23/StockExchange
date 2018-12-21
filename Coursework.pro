#-------------------------------------------------
#
# Project created by QtCreator 2018-12-05T13:23:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Coursework
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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    company.cpp \
    container.cpp \
    holder.cpp \
    holderwithanalysis.cpp \
    info.cpp \
    qcustomplot.cpp \
    graphwindow.cpp \
    companydialog.cpp \
    holderdialog.cpp

HEADERS += \
        mainwindow.h \
    company.h \
    container.h \
    holder.h \
    holderwithanalysis.h \
    info.h \
    qcustomplot.h \
    graphwindow.h \
    companydialog.h \
    holderdialog.h

FORMS += \
        mainwindow.ui \
    graphwindow.ui \
    companydialog.ui \
    holderdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
