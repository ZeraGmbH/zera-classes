#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

TEMPLATE = lib
CONFIG += plugin

QT       += network core
QT       -= gui

LIBS +=  -lzera-proxy
LIBS +=  -lzeramisc
LIBS +=  -lzeraxmlconfig
LIBS +=  -lzerabasemodule
LIBS +=  -lMeasurementModuleInterface

include(../zera-modules.pri)
include(../../zera-classes.pri)

INCLUDEPATH += ../../zera-basemodule \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
    ../../zera-validator \
    ../interface

TARGET = $$qtLibraryTarget(sec1module)

INCLUDEPATH += "src"

SOURCES += \
    src/sec1modulefactory.cpp \
    src/sec1moduleconfiguration.cpp \
    src/sec1modulemeasprogram.cpp \
    src/sec1module.cpp \

HEADERS += \
    src/sec1modulefactory.h \
    src/sec1module.h \
    src/sec1moduleconfiguration.h \
    src/sec1moduleconfigdata.h \
    src/sec1modulemeasprogram.h \
    src/debug.h \

OTHER_FILES += \
    src/sec1module.xml \
    src/sec1module.xsd

RESOURCES += \
    resources.qrc

