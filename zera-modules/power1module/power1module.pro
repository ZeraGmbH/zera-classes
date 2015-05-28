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
    ../../zera-basemodule


TARGET = $$qtLibraryTarget(power1module)


INCLUDEPATH += "src"

SOURCES += \
    src/power1module.cpp \
    src/power1moduleconfiguration.cpp \
    src/power1modulefactory.cpp \
    src/power1modulemeasprogram.cpp \
    src/power1moduleobservation.cpp \

HEADERS += \
    src/debug.h \
    src/power1module.h \
    src/power1moduleconfigdata.h \
    src/power1moduleconfiguration.h \
    src/power1modulefactory.h \
    src/power1modulemeasprogram.h \
    src/power1moduleobservation.h \

OTHER_FILES += \
    src/power1module.xml \
    src/power1module.xsd \
    src/power1module2.xml \
    src/power1module3.xml \
    src/power1moduleCED.xml \
    src/power1module2CED.xml \
    src/power1module3CED.xml

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)
