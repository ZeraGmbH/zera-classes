#include "main-unittest-qt.h"
#include "qt-unittest-sourceswitchjson.h"
#include "source-device/sourceio.h"
#include "source-device/sourceswitchjson.h"
#include "sourcedeviceerrorinjection-forunittest.h"

static QObject* pSourceIoTest = addTest(new SourceSwitchJsonTest);

void SourceSwitchJsonTest::init()
{
}

void SourceSwitchJsonTest::signalSwitch()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceIo::Ptr sourceIo = SourceIo::Ptr(new SourceIo(ioDevice, sourceProperties));
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIo);
    SourceSwitchJson switcher(sourceIo.get(), notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
}

void SourceSwitchJsonTest::signalSwitchAfterError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    ISourceIo::Ptr sourceIo = ISourceIo::Ptr(new SourceIo(ioDevice, sourceProperties));
    SourceIoErrorInjection *pSourceIoWithError = new SourceIoErrorInjection(sourceIo.get());
    ISourceIo::Ptr sourceIoWithError = ISourceIo::Ptr(pSourceIoWithError);
    pSourceIoWithError->setDemoResonseErrorIdx(0);
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIoWithError);
    SourceSwitchJson switcher(pSourceIoWithError, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
}

void SourceSwitchJsonTest::twoSignalsSwitchSameTwice()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    ISourceIo::Ptr sourceIo = ISourceIo::Ptr(new SourceIo(ioDevice, sourceProperties));
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIo);
    SourceSwitchJson switcher(sourceIo.get(), notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    switcher.switchState(paramState);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 2);
}

void SourceSwitchJsonTest::currentAndRequestedParamOnError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    ISourceIo::Ptr sourceIo = ISourceIo::Ptr(new SourceIo(ioDevice, sourceProperties));
    SourceIoErrorInjection *pSourceIoWithError = new SourceIoErrorInjection(sourceIo.get());
    ISourceIo::Ptr sourceIoWithError = ISourceIo::Ptr(pSourceIoWithError);
    pSourceIoWithError->setDemoResonseErrorIdx(0);
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIoWithError);
    SourceSwitchJson switcher(pSourceIoWithError, notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    JsonParamApi paramStateForError = paramState;
    paramStateForError.setOn(!paramState.getOn());

    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });
    switcher.switchState(paramStateForError);

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
    QVERIFY(paramStateForError.getParams() == switcher.getRequestedLoadState().getParams());
    QVERIFY(paramState.getParams() == switcher.getCurrLoadState().getParams());
}

void SourceSwitchJsonTest::changeParamOnSuccess()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    ISourceIo::Ptr sourceIo = ISourceIo::Ptr(new SourceIo(ioDevice, sourceProperties));
    SourceTransactionStartNotifier::Ptr notifyWrapperSwitch = SourceTransactionStartNotifier::Ptr::create(sourceIo);
    SourceSwitchJson switcher(sourceIo.get(), notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    JsonParamApi paramStateForError = paramState;
    paramStateForError.setOn(!paramState.getOn());

    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });
    switcher.switchState(paramStateForError);

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(paramChangeCount, 1);
    QVERIFY(paramState.getParams() != switcher.getCurrLoadState().getParams());
}
