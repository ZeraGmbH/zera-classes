#include "test_sourceio.h"
#include "test_globals.h"
#include "iodevicefactory.h"
#include "sourceio.h"
#include "jsonstructureloader.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"
#include <zera-json-params-state.h>

QTEST_MAIN(test_sourceio)

void test_sourceio::init()
{
    TimerFactoryQtForTest::enableTest();
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
                                      SourceProtocols::ZERA_SERIAL,
                                      SourceProperties::EXTERNAL);
    SourceIo sourceIo(ioDevice, sourceProperties);

    QCOMPARE(type, sourceIo.getProperties().getType());
    QCOMPARE(name, sourceIo.getProperties().getName());
    QCOMPARE(version, sourceIo.getProperties().getVersion());
    QCOMPARE(SourceProtocols::ZERA_SERIAL, sourceIo.getProperties().getProtocol());
    QCOMPARE(SourceProperties::EXTERNAL, sourceIo.getProperties().getLocation());
}

void test_sourceio::signalResponses()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceIo sourceIo(ioDevice, DefaultTestSourceProperties());

    int countResponseReceived = 0;
    connect(&sourceIo, &SourceIo::sigResponseReceived, [&] {
        countResponseReceived++;
    });

    IoQueueGroup::Ptr workTransferGroup1 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    sourceIo.startTransaction(workTransferGroup1);

    IoQueueGroup::Ptr workTransferGroup2 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    sourceIo.startTransaction(workTransferGroup2);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
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

    IoQueueGroup::Ptr workTransferGroup1 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    sourceIo.startTransaction(workTransferGroup1);

    IoQueueGroup::Ptr workTransferGroup2 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    sourceIo.startTransaction(workTransferGroup2);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
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

    IoQueueGroup::Ptr workTransferGroup1 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    sourceIo.startTransaction(workTransferGroup1);

    IoQueueGroup::Ptr workTransferGroup2 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    sourceIo.startTransaction(workTransferGroup2);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(countResponseReceived, 2);
}
