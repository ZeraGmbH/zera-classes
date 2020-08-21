cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-classes LANGUAGES CXX)

#Automoc and RCC ON. Needed in QT projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#C++ 11 Flag
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Set project Version
set(PROJECT_VERSION_MAJOR "1")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "9")

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(REPO_VERSION ${PROJECT_VERSION})

option(BUILD_MODULES "Build modules for modulemanager" ON)

if(BUILD_MODULES)
    set(SUB_DIRS_MODULES
        zera-modules/interface
        zera-validator
        zera-basemodule
        zera-xml-config
        zera-comm/zera-proxy
    )
endif(BUILD_MODULES)

set(SUB_DIRS
    ${SUB_DIRS_MODULES}
    zera-math
    zera-intel-hex-checker
    zera-mcontroller-io
    zera-comm/zera-i2c
    zera-dev
    zera-misc
)

set(MODULE_DIRS
    zera-modules/rangemodule
    zera-modules/rmsmodule
    zera-modules/dftmodule
    zera-modules/power1module
    zera-modules/thdnmodule
    zera-modules/oscimodule
    zera-modules/fftmodule
    zera-modules/samplemodule
    zera-modules/referencemodule
    zera-modules/modemodule
    zera-modules/sec1module
    zera-modules/scpimodule
    zera-modules/power2module
    zera-modules/power3module
    zera-modules/transformer1module
    zera-modules/burden1module
    zera-modules/efficiency1module
    zera-modules/statusmodule
    zera-modules/lambdamodule
    zera-modules/adjustmentmodule
    zera-modules/sem1module
    zera-modules/spm1module
)


# zera_xml-config has an src folder with header files inside
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/zera-xml-config/src)

# add all subfolders to include dirs
foreach(TMP_DIR ${SUB_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${TMP_DIR})
    add_subdirectory(${TMP_DIR})
endforeach()

if(BUILD_MODULES)
    # add module dirs for build
    foreach(TMP_DIR ${MODULE_DIRS})
        add_subdirectory(${TMP_DIR})
    endforeach()
endif(BUILD_MODULES)






