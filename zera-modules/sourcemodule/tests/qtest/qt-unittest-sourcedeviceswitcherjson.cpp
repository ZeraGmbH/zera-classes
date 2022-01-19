#include "main-unittest-qt.h"
#include "qt-unittest-sourcedeviceswitcherjson.h"
#include "source-device/sourcedevice.h"
#include "source-device/sourcedeviceswitcherjson.h"

static QObject* pSourceDeviceTest = addTest(new SourceDeviceSwitcherJsonTest);

void SourceDeviceSwitcherJsonTest::init()
{
}

void SourceDeviceSwitcherJsonTest::signalSwitch()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 0);
    SourceDeviceSwitcherJson switcher(&sourceDevice);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceDeviceSwitcherJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 1);
}

void SourceDeviceSwitcherJsonTest::twoSignalsSwitchSameTwice()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 0);
    SourceDeviceSwitcherJson switcher(&sourceDevice);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceDeviceSwitcherJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    switcher.switchState(paramState);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 2);
}
