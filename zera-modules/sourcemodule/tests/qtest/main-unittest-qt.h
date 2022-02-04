#ifndef MAIN_UNITTEST_H
#define MAIN_UNITTEST_H

#include "io-device/iodevicebase.h"
#include "io-device/iodevicedemo.h"
#include <QObject>
#include <QList>
#include <QString>
#include <QtTest>
#include <vs_veinhash.h>

QObject* addTest(QObject* test);

IoDeviceBase::Ptr createOpenDemoIoDevice(QString deviceInfo = QString());
void setDemoIoFixedTimeout(IoDeviceBase::Ptr ioDevice, int timeoutMs);

VeinStorage::VeinHash *getNullEventSystem();

static constexpr int shortQtEventTimeout = 5;

#endif // MAIN_UNITTEST_H
