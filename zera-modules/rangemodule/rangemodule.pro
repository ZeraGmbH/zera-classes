#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(rangemodule.user.pri)

QT       += network core
QT       -= gui

LIBS +=  -lzera-proxy
LIBS +=  -lzeramisc
LIBS +=  -lzeraxmlconfig
LIBS +=  -lvein-qt
LIBS +=  -lMeasurementModuleInterface


INCLUDEPATH += ../interface \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \


TARGET = $$qtLibraryTarget(rangemodule)
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += "src"

SOURCES += \
    src/rangemodulefactory.cpp \
    src/basemodule.cpp \
    src/rangemodule.cpp \
    src/rangemoduleconfiguration.cpp \
    src/moduleparameter.cpp \
    src/basemeaschannel.cpp \
    src/rangemeaschannel.cpp \
    src/basemoduleconfiguration.cpp \
    src/moduleinfo.cpp \
    src/basemeasprogram.cpp \
    src/rangemodulemeasprogram.cpp \
    src/rangeobsermatic.cpp \
    src/adjustment.cpp \
    src/modulesignal.cpp \
    src/rangemoduleobservation.cpp \
    src/moduleactivist.cpp

HEADERS += \
    src/basemeaschannel.h \
    src/rangemodulefactory.h \
    src/basemodule.h \
    src/rangemodule.h \
    src/xmlsettings.h \
    src/rangemoduleconfiguration.h \
    src/rangemoduleconfigdata.h \
    src/rangemoduleinterfacedata.h \
    src/moduleparameter.h \
    src/rangemeaschannel.h \
    src/rangeinfo.h \
    src/basemoduleconfiguration.h \
    src/moduleinfo.h \
    src/basemeasprogram.h \
    src/rangemodulemeasprogram.h \
    src/rangeobsermatic.h \
    src/socket.h \
    src/adjustment.h \
    src/modulesignal.h \
    src/rangemoduleobservation.h \
    src/moduleactivist.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/rangemodule.xml \
    src/rangemodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
