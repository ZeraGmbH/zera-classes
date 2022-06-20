#ifndef TEST_GLOBALS_H
#define TEST_GLOBALS_H

#include <QtTest>
#include "io-device/iodevicebase.h"
#include "io-device/iodevicedemo.h"

static constexpr int shortQtEventTimeout = 5;

IoDeviceBase::Ptr createOpenDemoIoDevice(QString deviceInfo = QString());
void setDemoIoFixedTimeout(IoDeviceBase::Ptr ioDevice, int timeoutMs);

#endif // TEST_GLOBALS_H
