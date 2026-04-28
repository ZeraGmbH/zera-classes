#include "test_sourceswitchjson.h"
#include "test_globals.h"
#include "sourceioextserial.h"
#include "sourceswitchjsonextserial.h"
#include "sourcedeviceerrorinjection-forunittest.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"
#include <QSignalSpy>

QTEST_MAIN(test_sourceswitchjson)

void test_sourceswitchjson::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_sourceswitchjson::signalSwitch()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    DefaultTestSourceProperties sourceProperties;
    AbstractSourceIoPtr sourceIo = std::make_shared<SourceIoExtSerial>(ioDevice, sourceProperties);
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIo);
    SourceSwitchJsonExtSerial switcher(sourceIo, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadpoint();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJsonExtSerial::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
}

void test_sourceswitchjson::signalSwitchAfterError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    AbstractSourceIoPtr sourceIo = std::make_shared<SourceIoExtSerial>(ioDevice, DefaultTestSourceProperties());
    SourceIoErrorInjection *pSourceIoWithError = new SourceIoErrorInjection(sourceIo);
    AbstractSourceIoPtr sourceIoWithError = AbstractSourceIoPtr(pSourceIoWithError);
    pSourceIoWithError->setDemoResonseErrorIdx(0);
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIoWithError);
    SourceSwitchJsonExtSerial switcher(sourceIoWithError, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadpoint();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJsonExtSerial::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
}

void test_sourceswitchjson::twoSignalsSwitchSameTwice()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    AbstractSourceIoPtr sourceIo = std::make_shared<SourceIoExtSerial>(ioDevice, DefaultTestSourceProperties());
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIo);
    SourceSwitchJsonExtSerial switcher(sourceIo, notifyWrapperSwitch);

    QSignalSpy spy(&switcher, &SourceSwitchJsonExtSerial::sigSwitchFinished);
    JsonParamApi paramState = switcher.getCurrLoadpoint();
    int switchId1 = switcher.switchState(paramState);
    int switchId2 = switcher.switchState(paramState);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy[0][0], true);
    QCOMPARE(spy[0][1], switchId1);
    QCOMPARE(spy[1][0], true);
    QCOMPARE(spy[1][1], switchId2);
}

void test_sourceswitchjson::currentAndRequestedParamOnError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    AbstractSourceIoPtr sourceIo = std::make_shared<SourceIoExtSerial>(ioDevice, DefaultTestSourceProperties());
    SourceIoErrorInjection *pSourceIoWithError = new SourceIoErrorInjection(sourceIo);
    AbstractSourceIoPtr sourceIoWithError = AbstractSourceIoPtr(pSourceIoWithError);
    pSourceIoWithError->setDemoResonseErrorIdx(0);
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIoWithError);
    SourceSwitchJsonExtSerial switcher(sourceIoWithError, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadpoint();
    JsonParamApi paramStateForError = paramState;
    paramStateForError.setOn(!paramState.getOn());

    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJsonExtSerial::sigSwitchFinished, [&] {
        paramChangeCount++;
    });
    switcher.switchState(paramStateForError);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
    QVERIFY(paramStateForError.getParams() == switcher.getRequestedLoadState().getParams());
    QVERIFY(paramState.getParams() == switcher.getCurrLoadpoint().getParams());
}

void test_sourceswitchjson::changeParamOnSuccess()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    AbstractSourceIoPtr sourceIo = std::make_shared<SourceIoExtSerial>(ioDevice, DefaultTestSourceProperties());
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIo);
    SourceSwitchJsonExtSerial switcher(sourceIo, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadpoint();
    JsonParamApi paramStateForError = paramState;
    paramStateForError.setOn(!paramState.getOn());

    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJsonExtSerial::sigSwitchFinished, [&] {
        paramChangeCount++;
    });
    switcher.switchState(paramStateForError);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
    QVERIFY(paramState.getParams() != switcher.getCurrLoadpoint().getParams());
}
