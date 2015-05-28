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

INCLUDEPATH += ../../zera-basemodule \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
    ../interface


TARGET = $$qtLibraryTarget(rmsmodule)


INCLUDEPATH += "src"

SOURCES += \
    src/rmsmoduleconfiguration.cpp \
    src/rmsmodulefactory.cpp \
    src/rmsmodule.cpp \
    src/rmsmodulemeasprogram.cpp \
    src/rmsmoduleobservation.cpp \

HEADERS += \
    src/rmsmoduleconfiguration.h \
    src/rmsmoduleconfigdata.h \
    src/rmsmodulemeasprogram.h \
    src/rmsmodulefactory.h \
    src/rmsmodule.h \
    src/rmsmoduleobservation.h \
    src/debug.h

OTHER_FILES += \
    src/rmsmoduleREF.xml \
    src/rmsmoduleCED.xml \
    src/rmsmodule.xml \
    src/rmsmodule.xsd

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)
