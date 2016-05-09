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


TARGET = $$qtLibraryTarget(power2module)


INCLUDEPATH += "src"

SOURCES += \
    src/power2module.cpp \
    src/power2moduleconfiguration.cpp \
    src/power2modulefactory.cpp \
    src/power2modulemeasprogram.cpp \
    src/power2moduleobservation.cpp \

HEADERS += \
    src/debug.h \
    src/power2module.h \
    src/power2moduleconfigdata.h \
    src/power2moduleconfiguration.h \
    src/power2modulefactory.h \
    src/power2modulemeasprogram.h \
    src/power2moduleobservation.h \

OTHER_FILES += \
    src/power2module.xsd \
    src/power2moduleCED.xml

RESOURCES += \
    resources.qrc

