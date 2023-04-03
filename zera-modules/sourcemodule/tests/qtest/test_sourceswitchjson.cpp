#include "test_sourceswitchjson.h"
#include "test_globals.h"
#include "sourceio.h"
#include "sourceswitchjson.h"
#include "sourcedeviceerrorinjection-forunittest.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"

QTEST_MAIN(test_sourceswitchjson)

void test_sourceswitchjson::init()
{
    TimerFactoryQtForTest::enableTest();
}

void test_sourceswitchjson::signalSwitch()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    DefaultTestSourceProperties sourceProperties;
    SourceIo::Ptr sourceIo = SourceIo::Ptr(new SourceIo(ioDevice, sourceProperties));
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIo);
    SourceSwitchJson switcher(sourceIo, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
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
    ISourceIo::Ptr sourceIo = ISourceIo::Ptr(new SourceIo(ioDevice, DefaultTestSourceProperties()));
    SourceIoErrorInjection *pSourceIoWithError = new SourceIoErrorInjection(sourceIo);
    ISourceIo::Ptr sourceIoWithError = ISourceIo::Ptr(pSourceIoWithError);
    pSourceIoWithError->setDemoResonseErrorIdx(0);
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIoWithError);
    SourceSwitchJson switcher(sourceIoWithError, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
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
    ISourceIo::Ptr sourceIo = ISourceIo::Ptr(new SourceIo(ioDevice, DefaultTestSourceProperties()));
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIo);
    SourceSwitchJson switcher(sourceIo, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    switcher.switchState(paramState);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 2);
}

void test_sourceswitchjson::currentAndRequestedParamOnError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    ISourceIo::Ptr sourceIo = ISourceIo::Ptr(new SourceIo(ioDevice, DefaultTestSourceProperties()));
    SourceIoErrorInjection *pSourceIoWithError = new SourceIoErrorInjection(sourceIo);
    ISourceIo::Ptr sourceIoWithError = ISourceIo::Ptr(pSourceIoWithError);
    pSourceIoWithError->setDemoResonseErrorIdx(0);
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIoWithError);
    SourceSwitchJson switcher(sourceIoWithError, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    JsonParamApi paramStateForError = paramState;
    paramStateForError.setOn(!paramState.getOn());

    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });
    switcher.switchState(paramStateForError);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
    QVERIFY(paramStateForError.getParams() == switcher.getRequestedLoadState().getParams());
    QVERIFY(paramState.getParams() == switcher.getCurrLoadState().getParams());
}

void test_sourceswitchjson::changeParamOnSuccess()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    ISourceIo::Ptr sourceIo = ISourceIo::Ptr(new SourceIo(ioDevice, DefaultTestSourceProperties()));
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIo);
    SourceSwitchJson switcher(sourceIo, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    JsonParamApi paramStateForError = paramState;
    paramStateForError.setOn(!paramState.getOn());

    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });
    switcher.switchState(paramStateForError);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
    QVERIFY(paramState.getParams() != switcher.getCurrLoadState().getParams());
}
