#ifndef MAIN_UNITTEST_H
#define MAIN_UNITTEST_H

#include "io-device/iodevicefactory.h"
#include <QObject>
#include <QList>
#include <QtTest>

QObject* addTest(QObject* test);

tIoDeviceShPtr createOpenDemoInterface();

#endif // MAIN_UNITTEST_H
