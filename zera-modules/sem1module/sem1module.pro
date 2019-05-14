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

TARGET = $$qtLibraryTarget(sec1module)

INCLUDEPATH += "src"

SOURCES += \
    src/sem1modulefactory.cpp \
    src/sem1moduleconfiguration.cpp \
    src/sem1modulemeasprogram.cpp \
    src/sem1module.cpp \

HEADERS += \
    src/sem1modulefactory.h \
    src/sem1module.h \
    src/sem1moduleconfiguration.h \
    src/sem1moduleconfigdata.h \
    src/sem1modulemeasprogram.h \
    src/debug.h \

OTHER_FILES += \
    src/sem1module.xsd \
    src/com5003-sem1module.xml \
    src/mt310s2-sem1module.xml

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)


