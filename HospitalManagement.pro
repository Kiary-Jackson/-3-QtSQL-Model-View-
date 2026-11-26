QT += core gui sql widgets

CONFIG += c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = HospitalManagement

SOURCES += \
    main.cpp \
    dbmanager.cpp \
    patientmodel.cpp \
    delegates.cpp \
    patientdialog.cpp \
    loginwidget.cpp \
    patientwidget.cpp \
    mainwindow.cpp

HEADERS += \
    config.h \
    dbmanager.h \
    patientmodel.h \
    delegates.h \
    patientdialog.h \
    loginwidget.h \
    patientwidget.h \
    mainwindow.h

# MSVC 下源码含中文，强制 UTF-8 编译
msvc: QMAKE_CXXFLAGS += /utf-8
