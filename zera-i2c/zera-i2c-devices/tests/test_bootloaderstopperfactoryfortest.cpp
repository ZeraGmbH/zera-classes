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
    QVector<bool> immediateSequence = QVector<bool>() << true << false;
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(immediateSequence);

    ZeraMControllerBootloaderStopperPtr stopper1 = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(nullptr, 42);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), false);

    ZeraMControllerBootloaderStopperPtr stopper2 = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(nullptr, 42);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_bootloaderstopperfactoryfortest::oneImmediate()
{
    QVector<bool> immediateSequence = QVector<bool>() << true;
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(immediateSequence);
    ZeraMControllerBootloaderStopperPtr stopper = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(nullptr, 42);

    QSignalSpy spy(stopper.get(), &ZeraMControllerBootloaderStopperInterface::sigAssumeBootloaderStopped);
    stopper->stopBootloader(42);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_bootloaderstopperfactoryfortest::oneDelayed()
{
    QVector<bool> immediateSequence = QVector<bool>() << false;
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(immediateSequence);
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
    QVector<bool> immediateSequence = QVector<bool>() << false << true;
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(immediateSequence);

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


