#include "main-unittest-qt.h"
#include "qt-unittest-sourceio.h"
#include "io-device/iodevicefactory.h"
#include "source-device/sourceio.h"
#include "json/jsonstructureloader.h"
#include <zera-json-params-state.h>

static QObject* pSourceIoTest = addTest(new SourceIoTest);

void SourceIoTest::init()
{
}

void SourceIoTest::gettersOK()
{
    QString name = "fooName";
    QString version = "fooVersion";
    SupportedSourceTypes type = SOURCE_MT_COMMON;
    QString info = "fooInfo";

    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    ioDevice->open(info);

    SourceProperties sourceProperties(SOURCE_MT_COMMON, name, version);
    SourceIo sourceDevice(ioDevice, sourceProperties);

    QCOMPARE(type, sourceDevice.getProperties().getType());
    QCOMPARE(name, sourceDevice.getProperties().getName());
    QCOMPARE(version, sourceDevice.getProperties().getVersion());
}

void SourceIoTest::signalResponses()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceIo sourceDevice(ioDevice, sourceProperties);

    int countResponseReceived = 0;
    connect(&sourceDevice, &SourceIo::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    sourceDevice.startTransaction(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    sourceDevice.startTransaction(workTransferGroup2);

    QTest::qWait(10);
    QCOMPARE(countResponseReceived, 2);
}

void SourceIoTest::signalResponsesOnOneError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceIo sourceDevice(ioDevice, sourceProperties);

    int countResponseReceived = 0;
    connect(&sourceDevice, &SourceIo::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    sourceDevice.startTransaction(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    sourceDevice.startTransaction(workTransferGroup2);

    QTest::qWait(10);
    QCOMPARE(countResponseReceived, 2);
}

void SourceIoTest::signalResponsesOnTwoErrors()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceProperties sourceProperties(SOURCE_MT_COMMON, "", "");
    SourceIo sourceDevice(ioDevice, sourceProperties);

    int countResponseReceived = 0;
    connect(&sourceDevice, &SourceIo::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    sourceDevice.startTransaction(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    sourceDevice.startTransaction(workTransferGroup2);

    QTest::qWait(10);
    QCOMPARE(countResponseReceived, 2);
}
