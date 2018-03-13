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


TARGET = $$qtLibraryTarget(power1module)


INCLUDEPATH += "src"

SOURCES += \
    src/power1module.cpp \
    src/power1moduleconfiguration.cpp \
    src/power1modulefactory.cpp \
    src/power1modulemeasprogram.cpp \
    src/power1moduleobservation.cpp \

HEADERS += \
    src/debug.h \
    src/power1module.h \
    src/power1moduleconfigdata.h \
    src/power1moduleconfiguration.h \
    src/power1modulefactory.h \
    src/power1modulemeasprogram.h \
    src/power1moduleobservation.h \

OTHER_FILES += \
    src/power1module.xsd \
    src/com5003-power1module.xml \
    src/com5003-power1module2.xml \
    src/com5003-power1module3.xml \
    src/com5003-power1moduleCED.xml \
    src/com5003-power1module2CED.xml \
    src/com5003-power1module3CED.xml \
    src/com5003-power1module4.xml \
    src/mt310s2-power1module.xml \
    src/mt310s2-power1module2.xml \
    src/mt310s2-power1module3.xml \
    src/com5003-power1moduleREF.xml

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)

