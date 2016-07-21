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

INCLUDEPATH += ../../zera-basemodule \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
    ../../zera-validator \
    ../interface

TARGET = $$qtLibraryTarget(modemodule)


INCLUDEPATH += "src"

SOURCES += \
    src/modemodulefactory.cpp \
    src/modemoduleconfiguration.cpp \
    src/modemodule.cpp \
    src/modemoduleinit.cpp

HEADERS += \
    src/modemodulefactory.h \
    src/modemodule.h \
    src/modemoduleconfiguration.h \
    src/modemoduleconfigdata.h \
    src/debug.h \
    src/modemoduleinit.h

OTHER_FILES += \
    src/modemodule.xsd \
    src/modemoduleAC.xml \
    src/modemoduleREF.xml

RESOURCES += \
    resource.qrc

include(../../zera-classes.pri)
