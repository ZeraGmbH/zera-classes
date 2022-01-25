#ifndef MAIN_UNITTEST_H
#define MAIN_UNITTEST_H

#include "io-device/iodevicebase.h"
#include "io-device/iodevicedemo.h"
#include <QObject>
#include <QList>
#include <QtTest>

QObject* addTest(QObject* test);

IoDeviceBase::Ptr createOpenDemoIoDevice();
void setDemoIoFixedTimeout(IoDeviceBase::Ptr ioDevice, int timeoutMs);

#endif // MAIN_UNITTEST_H
