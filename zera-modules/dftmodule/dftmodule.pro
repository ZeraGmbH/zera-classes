#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(dftmodule.user.pri)

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


TARGET = $$qtLibraryTarget(dftmodule)
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += "src"

SOURCES += \
    src/basemodule.cpp \
    src/moduleparameter.cpp \
    src/basemoduleconfiguration.cpp \
    src/moduleinfo.cpp \
    src/basemeasprogram.cpp \
    src/modulesignal.cpp \
    src/moduleactivist.cpp \
    src/dftmodule.cpp \
    src/dftmodulefactory.cpp \
    src/dftmoduleconfiguration.cpp \
    src/dftmodulemeasprogram.cpp \
    src/dftmoduleobservation.cpp \
    src/moduleerror.cpp

HEADERS += \
    src/basemodule.h \
    src/xmlsettings.h \
    src/moduleparameter.h \
    src/basemoduleconfiguration.h \
    src/moduleinfo.h \
    src/basemeasprogram.h \
    src/socket.h \
    src/modulesignal.h \
    src/moduleactivist.h \
    src/measchannelinfo.h \
    src/reply.h \
    src/dftmodule.h \
    src/dftmoduleconfigdata.h \
    src/dftmoduleconfiguration.h \
    src/dftmodulefactory.h \
    src/dftmodulemeasprogram.h \
    src/dftmoduleobservation.h \
    src/debug.h \
    src/errormessages.h \
    src/moduleerror.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/dftmodule.xml \
    src/dftmodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
