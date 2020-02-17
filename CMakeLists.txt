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
set(PROJECT_VERSION_PATCH "8")

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(REPO_VERSION ${PROJECT_VERSION})



set(SUB_DIRS
    zera-math
    zera-comm/zera-i2c
    zera-dev
    zera-xml-config
    zera-comm/zera-proxy
    zera-misc
    zera-modules/interface
    zera-validator
    zera-basemodule

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


#zera_xml-config has an src folder with header files inside
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/zera-xml-config/src)

#add all subfolders to include dirs
foreach(TMP_DIR ${SUB_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${TMP_DIR})
add_subdirectory(${TMP_DIR})
endforeach()

#add module dirs for build
foreach(TMP_DIR ${MODULE_DIRS})
add_subdirectory(${TMP_DIR})
endforeach()






