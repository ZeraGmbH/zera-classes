#include "main-unittest-qt.h"
#include "qt-unittest-sourcedeviceswitcherjson.h"
#include "device/sourcedevice.h"
#include "device/sourcedeviceswitcherjson.h"

static QObject* pSourceDeviceTest = addTest(new SourceDeviceSwitcherJsonTest);

void SourceDeviceSwitcherJsonTest::init()
{
}

void SourceDeviceSwitcherJsonTest::signalSwitch()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 0);
    SourceDeviceSwitcherJson switcher(&sourceDevice);

    JsonParamApi paramState = switcher.getCurrParamState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceDeviceSwitcherJson::sigCurrParamTouched, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 1);
}

void SourceDeviceSwitcherJsonTest::twoSignalsSwitchSameTwice()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 0);
    SourceDeviceSwitcherJson switcher(&sourceDevice);

    JsonParamApi paramState = switcher.getCurrParamState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceDeviceSwitcherJson::sigCurrParamTouched, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    switcher.switchState(paramState);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 2);
}
