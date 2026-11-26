QT += core sql
QT -= gui

CONFIG += console c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = test_db

SOURCES += \
    test_db.cpp \
    ../dbmanager.cpp \
    ../patientmodel.cpp

HEADERS += \
    ../config.h \
    ../dbmanager.h \
    ../patientmodel.h

msvc: QMAKE_CXXFLAGS += /utf-8
