#include "test_sourceio.h"
#include "test_globals.h"
#include "sourceioextserial.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"
#include <zera-json-params-state.h>

QTEST_MAIN(test_sourceio)

void test_sourceio::init()
{
    TimerFactoryQtForTest::enableTest();
}

void test_sourceio::signalResponses()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceIoExtSerial sourceIo(ioDevice, DefaultTestSourceProperties());

    int countResponseReceived = 0;
    connect(&sourceIo, &SourceIoExtSerial::sigResponseReceived, [&] {
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
    SourceIoExtSerial sourceIo(ioDevice, DefaultTestSourceProperties());

    int countResponseReceived = 0;
    connect(&sourceIo, &SourceIoExtSerial::sigResponseReceived, [&] {
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
    SourceIoExtSerial sourceIo(ioDevice, DefaultTestSourceProperties());

    int countResponseReceived = 0;
    connect(&sourceIo, &SourceIoExtSerial::sigResponseReceived, [&] {
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
