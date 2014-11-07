#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(oscimodule.user.pri)

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


TARGET = $$qtLibraryTarget(oscimodule)
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += "src"

SOURCES += \
    src/oscimodule.cpp \
    src/oscimoduleconfiguration.cpp \
    src/oscimodulefactory.cpp \
    src/oscimodulemeasprogram.cpp \
    src/oscimoduleobservation.cpp

HEADERS += \
    src/debug.h \
    src/oscimodule.h \
    src/oscimoduleconfigdata.h \
    src/oscimoduleconfiguration.h \
    src/oscimodulefactory.h \
    src/oscimodulemeasprogram.h \
    src/oscimoduleobservation.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/oscimodule.xml \
    src/oscimodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
