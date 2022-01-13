#include "main-unittest-qt.h"
#include "qt-unittest-sourcedeviceswitcherjson.h"
#include "device/sourcedevice.h"
#include "device/sourcedeviceswitcherjson.h"

static QObject* pSourceDeviceTest = addTest(new SourceJsonSwitcherTest);

void SourceJsonSwitcherTest::init()
{
}

void SourceJsonSwitcherTest::signalSwitch()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 0);
    SourceDeviceSwitcherJson switcher(&sourceDevice);

    QJsonObject json = switcher.getCurrParamState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceDeviceSwitcherJson::sigCurrParamTouched, [&] {
        paramChangeCount++;
    });

    switcher.switchState(json);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 1);
}

void SourceJsonSwitcherTest::twoSignalsSwitchSameTwice()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 0);
    SourceDeviceSwitcherJson switcher(&sourceDevice);

    QJsonObject json = switcher.getCurrParamState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceDeviceSwitcherJson::sigCurrParamTouched, [&] {
        paramChangeCount++;
    });

    switcher.switchState(json);
    switcher.switchState(json);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 2);
}

static void getBusyToggleCount(bool ioResponseDelay) {
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, ioResponseDelay ? 50 : 0);
    SourceDeviceSwitcherJson switcher(&sourceDevice);

    QJsonObject json = switcher.getCurrParamState();

    int countSwitches = 2;
    switcher.switchState(json);
    switcher.switchState(json);

    int busyToggleCount = 0;
    QObject::connect(&switcher, &SourceDeviceSwitcherJson::sigBusyChanged, [&] (bool busy) {
        if(busyToggleCount < countSwitches) {
            QVERIFY(busy);
        }
        else {
            QVERIFY(!busy);
        }
        busyToggleCount++;
    });

    QTest::qWait(10);
    QCOMPARE(busyToggleCount, countSwitches +(!ioResponseDelay ? countSwitches : 0) );
}

void SourceJsonSwitcherTest::busyToggledOnSwitch()
{
    getBusyToggleCount(false);
}

void SourceJsonSwitcherTest::ioLastLongerThanLifetime()
{
    getBusyToggleCount(true);
}

