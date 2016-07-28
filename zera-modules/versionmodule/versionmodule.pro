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

TARGET = $$qtLibraryTarget(versionmodule)


INCLUDEPATH += "src"

SOURCES += \
    src/versionmodulefactory.cpp \
    src/versionmoduleconfiguration.cpp \
    src/versionmodule.cpp \
    src/versionmoduleinit.cpp

HEADERS += \
    src/versionmodulefactory.h \
    src/versionmodule.h \
    src/versionmoduleconfiguration.h \
    src/versionmoduleconfigdata.h \
    src/debug.h \
    src/versionmoduleinit.h

OTHER_FILES += \
    src/versionmodule.xsd \
    src/versionmodule.xml

RESOURCES += \
    resource.qrc


