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

INCLUDEPATH += ../interface \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
    ../../zera-validator \
    ../../zera-basemodule


TARGET = $$qtLibraryTarget(efficiency1module)


INCLUDEPATH += "src"

SOURCES += \
    src/efficiency1module.cpp \
    src/efficiency1moduleconfiguration.cpp \
    src/efficiency1modulefactory.cpp \
    src/efficiency1modulemeasprogram.cpp \
    src/efficiency1measdelegate.cpp

HEADERS += \
    src/debug.h \
    src/efficiency1module.h \
    src/efficiency1moduleconfigdata.h \
    src/efficiency1moduleconfiguration.h \
    src/efficiency1modulefactory.h \
    src/efficiency1modulemeasprogram.h \
    src/efficiency1measdelegate.h

OTHER_FILES += \
    src/efficiency1module.xml \
    src/efficiency1module.xsd
    

RESOURCES += \
    resources.qrc


