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

TARGET = $$qtLibraryTarget(statusmodule)


INCLUDEPATH += "src"

SOURCES += \
    src/statusmodule.cpp \
    src/statusmoduleconfiguration.cpp \
    src/statusmodulefactory.cpp \
    src/statusmoduleinit.cpp

HEADERS += \
    src/debug.h \
    src/statusmodule.h \
    src/statusmoduleconfigdata.h \
    src/statusmoduleconfiguration.h \
    src/statusmodulefactory.h \
    src/statusmoduleinit.h

OTHER_FILES += \
    src/statusmodule.xsd \
    src/statusmodule.xml

RESOURCES += \
    resource.qrc

DISTFILES += \
    src/statusmodule.xml

include(../../zera-classes.pri)
