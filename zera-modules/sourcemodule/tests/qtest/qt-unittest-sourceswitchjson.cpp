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
    SourceIo sourceDevice(ioDevice, sourceProperties);
    SourceTransactionStartNotifier notifyWrapperSwitch(&sourceDevice);
    SourceSwitchJson switcher(&sourceDevice, &notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 1);
}

void SourceSwitchJsonTest::signalSwitchAfterError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceIo sourceDevice(ioDevice, sourceProperties);
    SourceDeviceErrorInjection sourceDeviceWithError(&sourceDevice);
    sourceDeviceWithError.setDemoResonseErrorIdx(0);
    SourceTransactionStartNotifier notifyWrapperSwitch(&sourceDeviceWithError);
    SourceSwitchJson switcher(&sourceDeviceWithError, &notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 1);
}

void SourceSwitchJsonTest::twoSignalsSwitchSameTwice()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceIo sourceDevice(ioDevice, sourceProperties);
    SourceTransactionStartNotifier notifyWrapperSwitch(&sourceDevice);
    SourceSwitchJson switcher(&sourceDevice, &notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });

    switcher.switchState(paramState);
    switcher.switchState(paramState);
    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 2);
}

void SourceSwitchJsonTest::currentAndRequestedParamOnError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceIo sourceDevice(ioDevice, sourceProperties);
    SourceDeviceErrorInjection sourceDeviceWithError(&sourceDevice);
    sourceDeviceWithError.setDemoResonseErrorIdx(0);
    SourceTransactionStartNotifier notifyWrapperSwitch(&sourceDeviceWithError);
    SourceSwitchJson switcher(&sourceDeviceWithError, &notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    JsonParamApi paramStateForError = paramState;
    paramStateForError.setOn(!paramState.getOn());

    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });
    switcher.switchState(paramStateForError);

    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 1);
    QVERIFY(paramStateForError.getParams() == switcher.getRequestedLoadState().getParams());
    QVERIFY(paramState.getParams() == switcher.getCurrLoadState().getParams());
}

void SourceSwitchJsonTest::changeParamOnSuccess()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    setDemoIoFixedTimeout(ioDevice, 0);
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceIo sourceDevice(ioDevice, sourceProperties);
    SourceTransactionStartNotifier notifyWrapperSwitch(&sourceDevice);
    SourceSwitchJson switcher(&sourceDevice, &notifyWrapperSwitch);

    JsonParamApi paramState = switcher.getCurrLoadState();
    JsonParamApi paramStateForError = paramState;
    paramStateForError.setOn(!paramState.getOn());

    int paramChangeCount = 0;
    connect(&switcher, &SourceSwitchJson::sigSwitchFinished, [&] {
        paramChangeCount++;
    });
    switcher.switchState(paramStateForError);

    QTest::qWait(10);
    QCOMPARE(paramChangeCount, 1);
    QVERIFY(paramState.getParams() != switcher.getCurrLoadState().getParams());
}
