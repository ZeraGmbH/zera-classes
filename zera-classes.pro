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
            zera-validator \
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
            zera-modules/modemodule \
            zera-modules/sec1module \
            zera-modules/scpimodule \
            zera-modules/power2module \
            zera-modules/power3module \
            zera-modules/transformer1module \
            zera-modules/burden1module \
            zera-modules/efficiency1module \
            zera-modules/statusmodule
