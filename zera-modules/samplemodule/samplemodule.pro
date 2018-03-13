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
    src/samplemodule.xsd

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)

DISTFILES += \
    src/com5003-samplemodule.xml \
    src/mt310s2-samplemodule.xml
