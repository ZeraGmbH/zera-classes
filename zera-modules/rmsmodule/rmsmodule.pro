#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(rmsmodule.user.pri)

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


TARGET = $$qtLibraryTarget(rmsmodule)
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
    src/rmsmoduleconfiguration.cpp \
    src/rmsmodulefactory.cpp \
    src/rmsmodule.cpp \
    src/rmsmodulemeasprogram.cpp \
    src/rmsmoduleobservation.cpp

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
    src/rmsmoduleconfiguration.h \
    src/rmsmoduleconfigdata.h \
    src/rmsmodulemeasprogram.h \
    src/rmsmodulefactory.h \
    src/rmsmodule.h \
    src/rmsmoduleobservation.h \
    src/measchannelinfo.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/rmsmodule.xml \
    src/rmsmodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
