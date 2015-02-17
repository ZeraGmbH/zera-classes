TEMPLATE    = subdirs

QMAKE_CXXFLAGS += -O0

CONFIG += ordered

SUBDIRS     = \
            zera-math \
            zera-meas \
            zera-comm/zera-i2c \
            zera-dev \
            zera-xml-config \
            zera-comm/zera-proxy \
            zera-misc \
            zera-modules/interface \
            zera-basemodule \
            zera-modules/rangemodule \
            zera-modules/rmsmodule \
            zera-modules/dftmodule \
            zera-modules/power1module \
            zera-modules/thdnmodule \
            zera-modules/oscimodule \
            zera-modules/fftmodule \
            zera-modules/samplemodule \
            zera-modules/referencemodule \
            zera-modules/modemodule



