#-------------------------------------------------
#
# Project created by QtCreator 2015-06-15T16:31:42
#
#-------------------------------------------------

TEMPLATE = lib

#dependencies
VEIN_DEP_EVENT = 1
VEIN_DEP_COMP = 1
VEIN_DEP_HASH = 1

#do not copy the target into the $$VEIN_BASEDIR/libs directory
VF_NO_DEPLOY = 1

include(../zera-classes.pri)

exists($$VEIN_BASEDIR/project-paths.pri) {
  include($$VEIN_BASEDIR/project-paths.pri)
}

QT       -= gui

TARGET = zera-validator


DEFINES += ZERAVALIDATOR_LIBRARY

SOURCES += \
    commandvalidator.cpp \
    groupvalidator.cpp

HEADERS += \
    zera-validator_global.h \
    commandvalidator.h \
    groupvalidator.h
