#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(fftmodule.user.pri)

QT       += network core
QT       -= gui

LIBS +=  -lzera-proxy
LIBS +=  -lzeramisc
LIBS +=  -lzeraxmlconfig
LIBS +=  -lvein-qt
LIBS +=  -lzerabasemodule
//LIBS +=  -L$$shadowed($$PWD/../../libs)


INCLUDEPATH += ../../zera-basemodule \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \


TARGET = $$qtLibraryTarget(fftmodule)
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += "src"

SOURCES += \
    src/fftmodule.cpp \
    src/fftmodulefactory.cpp \
    src/fftmoduleconfiguration.cpp \
    src/fftmodulemeasprogram.cpp \
    src/fftmoduleobservation.cpp

HEADERS += \
    src/fftmodule.h \
    src/fftmoduleconfigdata.h \
    src/fftmoduleconfiguration.h \
    src/fftmodulefactory.h \
    src/fftmodulemeasprogram.h \
    src/fftmoduleobservation.h \
    src/debug.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/fftmodule.xml \
    src/fftmodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
