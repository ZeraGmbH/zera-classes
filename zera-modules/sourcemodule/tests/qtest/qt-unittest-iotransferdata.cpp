#include "main-unittest-qt.h"
#include "qt-unittest-iotransferdata.h"
#include "io-queue/ioqueueentry.h"
#include "io-device/iodevicebase.h"

static QObject* pIoQueueTest = addTest(new IoTransferDataTest);

void IoTransferDataTest::init()
{
}

void IoTransferDataTest::singleDataEvalNotExecutedOnConstruct1()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create();
    QVERIFY(ioTransferData->wasNotRunYet());
}

void IoTransferDataTest::singleDataEvalNotExecutedOnConstruct2()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    QVERIFY(ioTransferData->wasNotRunYet());
}

void IoTransferDataTest::singleDemoResponseLeadTrail()
{
    QByteArray lead = "foo";
    QByteArray trail = "foo";
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", lead, trail);
    QCOMPARE(ioTransferData->getDemoResponse(), lead+trail);
}

void IoTransferDataTest::singleDemoResponseLeadTrailEmpty()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create();
    QCOMPARE(ioTransferData->getDemoResponse(), "\r");
}

void IoTransferDataTest::singleDemoResponseSimError()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create();
    ioTransferData->m_demoErrorResponse = true;
    QCOMPARE(ioTransferData->getDemoResponse(), IoTransferDataSingle::demoErrorResponseData);
}

void IoTransferDataTest::singleCheckUnusedDataOnOnConstruct1()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create();
    QCOMPARE(ioTransferData->wasNotRunYet(), true);
}

void IoTransferDataTest::singleCheckUnusedDataOnOnConstruct2()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    QCOMPARE(ioTransferData->wasNotRunYet(), true);
}

void IoTransferDataTest::singleCheckUsedDataDataReceived()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create();
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void IoTransferDataTest::singleCheckUsedDataNoAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "", "");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->noAnswerReceived());
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void IoTransferDataTest::singleCheckUsedWrongAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create();
    ioTransferData->m_demoErrorResponse = true;
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->wrongAnswerReceived());
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void IoTransferDataTest::singleCheckUsedPass()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("fooSend", "fooLead", "fooTrail");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->didIoPass());
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void IoTransferDataTest::groupIdsCreated()
{
    IoQueueEntry workGroup1(IoQueueEntry::BEHAVE_STOP_ON_ERROR);
    IoQueueEntry workGroup2(IoQueueEntry::BEHAVE_STOP_ON_ERROR);
    IoQueueEntry workGroup3(IoQueueEntry::BEHAVE_STOP_ON_ERROR);

    QVERIFY(workGroup1.getGroupId() != workGroup2.getGroupId());
    QVERIFY(workGroup2.getGroupId() != workGroup3.getGroupId());
    QVERIFY(workGroup3.getGroupId() != workGroup1.getGroupId());
}

void IoTransferDataTest::groupPassedAllFail()
{
    IoQueueEntry::Ptr workTransferGroup =
            IoQueueEntry::Ptr::create(IoQueueEntry::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create());
    transList1.append(IoTransferDataSingle::Ptr::create());
    workTransferGroup->appendTransferList(transList1);

    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create());
    transList2.append(IoTransferDataSingle::Ptr::create());
    workTransferGroup->appendTransferList(transList2);

    // not run communication -> all fail
    // single evaluation is done by io devices
    workTransferGroup->evalAll();
    QCOMPARE(workTransferGroup->passedAll(), false);
}

void IoTransferDataTest::singleDataEvalNotExecuted()
{
    IoTransferDataSingle::Ptr transSingle = IoTransferDataSingle::Ptr::create("", "");
    QVERIFY(transSingle->wasNotRunYet());
}

void IoTransferDataTest::singleDataEvalNoAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "", "");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->noAnswerReceived());
}

void IoTransferDataTest::singleDataEvalWrongAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create();
    ioTransferData->m_demoErrorResponse = true;
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->wrongAnswerReceived());
}

void IoTransferDataTest::singleDataEvalPass()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("fooSend", "fooLead", "fooTrail");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->didIoPass());
}


