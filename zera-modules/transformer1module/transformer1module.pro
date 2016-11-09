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


TARGET = $$qtLibraryTarget(transformer1module)


INCLUDEPATH += "src"

SOURCES += \
    src/transformer1module.cpp \
    src/transformer1moduleconfiguration.cpp \
    src/transformer1modulefactory.cpp \
    src/transformer1modulemeasprogram.cpp \
    src/transformer1measdelegate.cpp

HEADERS += \
    src/debug.h \
    src/transformer1module.h \
    src/transformer1moduleconfigdata.h \
    src/transformer1moduleconfiguration.h \
    src/transformer1modulefactory.h \
    src/transformer1modulemeasprogram.h \
    src/transformer1measdelegate.h

OTHER_FILES += \
    src/transformer1module.xml \
    src/transformer1module.xsd


RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)
