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

TARGET = $$qtLibraryTarget(spm1module)

INCLUDEPATH += "src"

SOURCES += \
    src/spm1modulefactory.cpp \
    src/spm1moduleconfiguration.cpp \
    src/spm1modulemeasprogram.cpp \
    src/spm1module.cpp \

HEADERS += \
    src/spm1modulefactory.h \
    src/spm1module.h \
    src/spm1moduleconfiguration.h \
    src/spm1moduleconfigdata.h \
    src/spm1modulemeasprogram.h \
    src/debug.h \

OTHER_FILES += \
    src/spm1module.xsd \
    src/com5003-spm1module.xml \
    src/mt310s2-spm1module.xml

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)


