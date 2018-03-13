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

INCLUDEPATH += ../interface \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
    ../../zera-validator \
    ../../zera-basemodule


TARGET = $$qtLibraryTarget(power3module)


INCLUDEPATH += "src"

SOURCES += \
    src/power3module.cpp \
    src/power3moduleconfiguration.cpp \
    src/power3modulefactory.cpp \
    src/power3modulemeasprogram.cpp \
    src/power3measdelegate.cpp

HEADERS += \
    src/debug.h \
    src/power3module.h \
    src/power3moduleconfigdata.h \
    src/power3moduleconfiguration.h \
    src/power3modulefactory.h \
    src/power3modulemeasprogram.h \
    src/power3measdelegate.h

OTHER_FILES += \
    src/power3module.xsd \
    src/com5003-power3module.xml \
    src/mt310s2-power3module.xml


RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)


