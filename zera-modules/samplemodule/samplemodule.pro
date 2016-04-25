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

TARGET = $$qtLibraryTarget(samplemodule)

INCLUDEPATH += "src"

SOURCES += \
    src/samplemodulefactory.cpp \
    src/samplemoduleconfiguration.cpp \
    src/samplechannel.cpp \
    src/samplemodulemeasprogram.cpp \
    src/pllobsermatic.cpp \
    src/samplemoduleobservation.cpp \
    src/samplemodule.cpp \
    src/pllmeaschannel.cpp

HEADERS += \
    src/samplemodulefactory.h \
    src/samplemodule.h \
    src/samplemoduleconfiguration.h \
    src/samplemoduleconfigdata.h \
    src/samplechannel.h \
    src/samplemodulemeasprogram.h \
    src/pllobsermatic.h \
    src/samplemoduleobservation.h \
    src/debug.h \
    src/pllmeaschannel.h

OTHER_FILES += \
    src/samplemodule.xml \
    src/samplemodule.xsd

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)
include(../zera-modules.pri)
