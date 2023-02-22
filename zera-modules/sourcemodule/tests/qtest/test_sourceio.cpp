#include "test_sourceio.h"
#include "test_globals.h"
#include "iodevicefactory.h"
#include "source-device/sourceio.h"
#include "json/jsonstructureloader.h"
#include <zera-json-params-state.h>

QTEST_MAIN(test_sourceio)

void test_sourceio::init()
{
}

void test_sourceio::gettersOK()
{
    QString name = "fooName";
    QString version = "fooVersion";
    SupportedSourceTypes type = SOURCE_MT_COMMON;
    QString info = "fooInfo";

    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    ioDevice->open(info);

    SourceProperties sourceProperties(SOURCE_MT_COMMON,
                                      name,
                                      version,
                                      SourceProtocols::ZERA_SERIAL);
    SourceIo sourceIo(ioDevice, sourceProperties);

    QCOMPARE(type, sourceIo.getProperties().getType());
    QCOMPARE(name, sourceIo.getProperties().getName());
    QCOMPARE(version, sourceIo.getProperties().getVersion());
    QCOMPARE(SourceProtocols::ZERA_SERIAL, sourceIo.getProperties().getProtocol());
}

void test_sourceio::signalResponses()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceIo sourceIo(ioDevice, DefaultTestSourceProperties());

    int countResponseReceived = 0;
    connect(&sourceIo, &SourceIo::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    sourceIo.startTransaction(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    sourceIo.startTransaction(workTransferGroup2);

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(countResponseReceived, 2);
}

void test_sourceio::signalResponsesOnOneError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceIo sourceIo(ioDevice, DefaultTestSourceProperties());

    int countResponseReceived = 0;
    connect(&sourceIo, &SourceIo::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    sourceIo.startTransaction(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    sourceIo.startTransaction(workTransferGroup2);

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(countResponseReceived, 2);
}

void test_sourceio::signalResponsesOnTwoErrors()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceIo sourceIo(ioDevice, DefaultTestSourceProperties());

    int countResponseReceived = 0;
    connect(&sourceIo, &SourceIo::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    sourceIo.startTransaction(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    sourceIo.startTransaction(workTransferGroup2);

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(countResponseReceived, 2);
}
