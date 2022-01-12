#include "main-unittest-qt.h"
#include "qt-unittest-sourcejsonswitcher.h"
#include "device/sourcedevice.h"
#include "device/sourcejsonswitcher.h"

static QObject* pSourceDeviceTest = addTest(new SourceJsonSwitcherTest);

void SourceJsonSwitcherTest::init()
{
}

void SourceJsonSwitcherTest::signalSwitch()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 0);
    SourceJsonSwitcher switcher(&sourceDevice);

    QJsonObject json = switcher.getCurrParamState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceJsonSwitcher::sigCurrParamTouched, [&] {
        paramChangeCount++;
    });

    switcher.switchState(json);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 1);
}

void SourceJsonSwitcherTest::twoSignalsSwitchSameTwice()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, 0);
    SourceJsonSwitcher switcher(&sourceDevice);

    QJsonObject json = switcher.getCurrParamState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceJsonSwitcher::sigCurrParamTouched, [&] {
        paramChangeCount++;
    });

    switcher.switchState(json);
    switcher.switchState(json);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 2);
}

static void getBusyToggleCount(bool ioResponseDelay) {
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");
    SourceDevice sourceDevice(interface, SOURCE_MT_COMMON, "", "");
    sourceDevice.setDemoResponseDelay(false, ioResponseDelay ? 50 : 0);
    SourceJsonSwitcher switcher(&sourceDevice);

    QJsonObject json = switcher.getCurrParamState();

    int countSwitches = 2;
    switcher.switchState(json);
    switcher.switchState(json);

    int busyToggleCount = 0;
    QObject::connect(&switcher, &SourceJsonSwitcher::sigBusyChanged, [&] (bool busy) {
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

