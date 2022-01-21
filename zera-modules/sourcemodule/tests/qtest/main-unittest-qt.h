#ifndef MAIN_UNITTEST_H
#define MAIN_UNITTEST_H

#include "io-device/iodevicebase.h"
#include <QObject>
#include <QList>
#include <QtTest>

QObject* addTest(QObject* test);

IoDeviceBase::Ptr createOpenDemoInterface();

#endif // MAIN_UNITTEST_H
