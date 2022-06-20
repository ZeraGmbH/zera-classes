#ifndef MAIN_UNITTEST_H
#define MAIN_UNITTEST_H

#include "io-device/iodevicebase.h"
#include "io-device/iodevicedemo.h"
#include <QObject>
#include <QList>
#include <QString>
#include <QtTest>
#include <vs_veinhash.h>

typedef int (*Ttest)(int argc, char *argv[]);
Ttest addTestFunc(Ttest ptr);

#undef QTEST_MAIN
#define QTEST_MAIN(TestClass) \
    static int testFunc(int argc, char *argv[]) \
    { \
        QTEST_MAIN_IMPL(TestClass) \
    } \
    static Ttest dummy = addTestFunc(testFunc);

IoDeviceBase::Ptr createOpenDemoIoDevice(QString deviceInfo = QString());
void setDemoIoFixedTimeout(IoDeviceBase::Ptr ioDevice, int timeoutMs);

static constexpr int shortQtEventTimeout = 5;

#endif // MAIN_UNITTEST_H
