#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------
TEMPLATE = lib
CONFIG += plugin

ZDEPENDS += libvein

QT       += network core
QT       -= gui

LIBS +=  -lzera-proxy
LIBS +=  -lzeramisc
LIBS +=  -lzeraxmlconfig
LIBS +=  -lzerabasemodule
LIBS +=  -lMeasurementModuleInterface

INCLUDEPATH += ../interface \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
    ../../zera-basemodule \
    ../interface

TARGET = $$qtLibraryTarget(thdnmodule)


INCLUDEPATH += "src"

SOURCES += \
    src/thdnmodule.cpp \
    src/thdnmoduleconfiguration.cpp \
    src/thdnmodulefactory.cpp \
    src/thdnmodulemeasprogram.cpp \
    src/thdnmoduleobservation.cpp

HEADERS += \
    src/thdnmodule.h \
    src/thdnmoduleconfigdata.h \
    src/thdnmoduleconfiguration.h \
    src/thdnmodulefactory.h \
    src/thdnmodulemeasprogram.h \
    src/thdnmoduleobservation.h \
    src/debug.h

OTHER_FILES += \
    src/thdnmodule.xml \
    src/thdnmodule.xsd

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)
