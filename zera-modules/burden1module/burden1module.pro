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
LIBS +=  -lzeramath
LIBS +=  -lzeraxmlconfig
LIBS +=  -lzerabasemodule
LIBS +=  -lMeasurementModuleInterface

include(../zera-modules.pri)

INCLUDEPATH += ../interface \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-math \
    ../../zera-comm/zera-proxy \
    ../../zera-validator \
    ../../zera-basemodule


TARGET = $$qtLibraryTarget(burden1module)


INCLUDEPATH += "src"

SOURCES += \
    src/burden1module.cpp \
    src/burden1moduleconfiguration.cpp \
    src/burden1modulefactory.cpp \
    src/burden1modulemeasprogram.cpp \
    src/burden1measdelegate.cpp

HEADERS += \
    src/debug.h \
    src/burden1module.h \
    src/burden1moduleconfigdata.h \
    src/burden1moduleconfiguration.h \
    src/burden1modulefactory.h \
    src/burden1modulemeasprogram.h \
    src/burden1measdelegate.h

OTHER_FILES += \
    src/burden1module.xml \
    src/burden1module.xsd


RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)

DISTFILES += \
    src/burden1module2.xml
