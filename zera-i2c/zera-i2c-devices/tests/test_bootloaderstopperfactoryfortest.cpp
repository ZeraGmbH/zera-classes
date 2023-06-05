#include "test_bootloaderstopperfactoryfortest.h"
#include "zeramcontrollerbootloaderstopperfactoryfortest.h"
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_bootloaderstopperfactoryfortest)

void test_bootloaderstopperfactoryfortest::init()
{
    TimerFactoryQtForTest::enableTest();
}

void test_bootloaderstopperfactoryfortest::checkEmpty()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true << false);

    ZeraMControllerBootloaderStopperPtr stopper1 = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(nullptr, 42);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), false);

    ZeraMControllerBootloaderStopperPtr stopper2 = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(nullptr, 42);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_bootloaderstopperfactoryfortest::oneImmediate()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true);
    ZeraMControllerBootloaderStopperPtr stopper = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(nullptr, 42);

    QSignalSpy spy(stopper.get(), &ZeraMControllerBootloaderStopperInterface::sigAssumeBootloaderStopped);
    stopper->stopBootloader(42);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_bootloaderstopperfactoryfortest::oneDelayed()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false);
    ZeraMControllerBootloaderStopperPtr stopper = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(nullptr, 42);

    QSignalSpy spy(stopper.get(), &ZeraMControllerBootloaderStopperInterface::sigAssumeBootloaderStopped);
    stopper->stopBootloader(1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_bootloaderstopperfactoryfortest::oneDelayedOneImmediate()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false << true);
    ZeraMControllerBootloaderStopperPtr stopperDelayed = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(nullptr, 42);
    QSignalSpy spyDelayed(stopperDelayed.get(), &ZeraMControllerBootloaderStopperInterface::sigAssumeBootloaderStopped);
    stopperDelayed->stopBootloader(1000);
    QCOMPARE(spyDelayed.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spyDelayed.count(), 1);

    ZeraMControllerBootloaderStopperPtr stopperImmediate = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(nullptr, 42);
    QSignalSpy spyImmediate(stopperImmediate.get(), &ZeraMControllerBootloaderStopperInterface::sigAssumeBootloaderStopped);
    stopperImmediate->stopBootloader(1000);
    QCOMPARE(spyImmediate.count(), 1);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}


