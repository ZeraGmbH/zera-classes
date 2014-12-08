#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(samplemodule.user.pri)

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


TARGET = $$qtLibraryTarget(samplemodule)
TEMPLATE = lib
CONFIG += plugin

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

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/samplemodule.xml \
    src/samplemodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
