#-------------------------------------------------
#
# Project created by QtCreator 2013-11-27T13:25:24
#
#-------------------------------------------------
TEMPLATE = lib

include(../../zera-classes.pri)

QT       -= gui

TARGET = MeasurementModuleInterface
#CONFIG += staticlib

HEADERS += \
    virtualmodule.h \
    abstractmodulefactory.h

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-modules
INSTALLS += header_files
