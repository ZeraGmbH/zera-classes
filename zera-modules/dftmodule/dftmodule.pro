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
    ../interface/


TARGET = $$qtLibraryTarget(dftmodule)


INCLUDEPATH += "src"

SOURCES += \
    src/dftmodule.cpp \
    src/dftmodulefactory.cpp \
    src/dftmoduleconfiguration.cpp \
    src/dftmodulemeasprogram.cpp \
    src/dftmoduleobservation.cpp

HEADERS += \
    src/dftmodule.h \
    src/dftmoduleconfigdata.h \
    src/dftmoduleconfiguration.h \
    src/dftmodulefactory.h \
    src/dftmodulemeasprogram.h \
    src/dftmoduleobservation.h \
    src/debug.h

OTHER_FILES += \
    src/dftmodule.xml \
    src/dftmoduleREF.xml \
    src/dftmodule.xsd

RESOURCES += \
    resources.qrc

