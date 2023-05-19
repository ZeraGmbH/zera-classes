#include "test_booloaderstopper.h"
#include "zeramcontrollerbootloaderstopper.h"
#include "zeramcontrolleriofortest.h"
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_booloaderstopper)

void test_booloaderstopper::init()
{
    TimerFactoryQtForTest::enableTest();
    TimeMachineForTest::reset();
}

void test_booloaderstopper::applicationRunning()
{
    std::shared_ptr<ZeraMcontrollerIoForTest> i2cCtrl = std::make_shared<ZeraMcontrollerIoForTest>("foo", 1, 2);
    i2cCtrl->simulateApplicationRunnung();
    ZeraMControllerBootloaderStopper stopper(i2cCtrl, 42);
    QSignalSpy spy(&stopper, &ZeraMControllerBootloaderStopper::sigAssumeBootloaderStopped);
    // Application running: notify immediately
    stopper.stopBootloader(1000);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 42);
}

void test_booloaderstopper::bootloaderRunning()
{
    std::shared_ptr<ZeraMcontrollerIoForTest> i2cCtrl = std::make_shared<ZeraMcontrollerIoForTest>("foo", 1, 2);
    i2cCtrl->simulateBooloaderRunning();
    ZeraMControllerBootloaderStopper stopper(i2cCtrl, 67);
    QSignalSpy spy(&stopper, &ZeraMControllerBootloaderStopper::sigAssumeBootloaderStopped);
    // Bootloader running: notify after delay
    stopper.stopBootloader(1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(500);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(500);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 67);
}
