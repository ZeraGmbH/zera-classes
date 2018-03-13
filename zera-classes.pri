#enable c++11
QMAKE_CXXFLAGS += -std=c++0x
#enable usefull warnings
QMAKE_CXXFLAGS += -Woverloaded-virtual -Wshadow -Wold-style-cast -Wall
#for newer gcc versions: -Wsuggest-override

contains(DEFINES, OE_BUILD) {
  message(Openembedded build)
}
else {
  # paths for s.stirtzel work environment
  exists(/work/qt_projects/include/project-paths.pri) {
    include(/work/qt_projects/include/project-paths.pri)
  }

  exists($$PWD/autobuilder.pri) {
    include($$PWD/autobuilder.pri)
  }

  exists(/home/peter/C++/zera-classes-project/project-paths.pri) {
    include(/home/peter/C++/zera-classes-project/project-paths.pri)
  }
}


## Usage: TEMPLATE and in plugin cases CONFIG need to be defined before the include line for this .pri

LIBS += -L$$PWD/libs

#to be evaluated later: -Wshadow
QMAKE_CXXFLAGS += -Wall -Werror=ignored-qualifiers -Werror=cast-qual -Werror=return-type

contains( CONFIG, plugin ) {
  unix:equals(TEMPLATE, "lib") {
    DESTDIR = $$PWD/modules

    target.path = /usr/lib/zera-modules
    INSTALLS += target

    config_files.files = $$OTHER_FILES
    config_files.path = /etc/zera/modules
    INSTALLS += config_files
  }
} else:unix:equals(TEMPLATE, "lib") {
  DESTDIR = $$PWD/libs
  VERSION = 1.0.7

  target.path = /usr/lib
  INSTALLS += target
}

unix:equals(TEMPLATE, "staticlib") {
  DESTDIR = $$PWD/libs
}

### Currently no apps here
#unix:equals(TEMPLATE, "app") {
#  DESTDIR = $$PWD/bin
#
#  target.path = /usr/bin
#  INSTALLS += target
#}



## Usage: ZDEPENDS removes redundancy in dependency resolving, declare it before the line including this .pri

contains( ZDEPENDS, protonet) {
  error("INVALID DEPENDENCY")
}
contains( ZDEPENDS, libvein) {
  error("INVALID DEPENDENCY")
}
#Adds dependencies to the resourcemanager protobuf
contains( ZDEPENDS, resman) {
  INCLUDEPATH += $$RESOURCE_PROTOBUF_INCLUDEDIR
  LIBS += $$RESOURCE_PROTOBUF_LIBDIR
  LIBS +=  -lzera-resourcemanager-protobuf
  LIBS +=  -lprotobuf
}
#Adds dependencies to the SCPI
contains( ZDEPENDS, SCPI) {
  INCLUDEPATH += $$SCPI_INCLUDEDIR
  LIBS += $$SCPI_LIBDIR
  LIBS +=  -lSCPI
}

# do not copy any files to the VEIN_BASEDIR lib/bin directory
VF_NO_DEPLOY = 1

include($$VEIN_BASEDIR/vein-framework.pri)
