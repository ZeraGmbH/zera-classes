#include "main-unittest-qt.h"
#include "qt-unittest-iotransferdata.h"
#include "io-device/iotransferdatagroup.h"
#include "io-device/iotransferdatasinglefactory.h"
#include "io-device/iodevicebase.h"

static QObject* pIoQueueTest = addTest(new IoTransferDataTest);

void IoTransferDataTest::init()
{
}

void IoTransferDataTest::singleDataEvalNotExecutedOnConstruct1()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData();
    QVERIFY(ioTransferData->wasNotRunYet());
}

void IoTransferDataTest::singleDataEvalNotExecutedOnConstruct2()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData("", "");
    QVERIFY(ioTransferData->wasNotRunYet());
}

void IoTransferDataTest::singleDemoResponseLeadTrail()
{
    QByteArray lead = "foo";
    QByteArray trail = "foo";
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData("", lead, trail);
    QCOMPARE(ioTransferData->getDemoResponse(), lead+trail);
}

void IoTransferDataTest::singleDemoResponseLeadTrailEmpty()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData();
    QCOMPARE(ioTransferData->getDemoResponse(), "\r");
}

void IoTransferDataTest::singleDemoResponseSimError()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData();
    ioTransferData->m_demoErrorResponse = true;
    QCOMPARE(ioTransferData->getDemoResponse(), IoTransferDataSingle::demoErrorResponseData);
}

void IoTransferDataTest::singleCheckUnusedDataOnOnConstruct1()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData();
    QCOMPARE(ioTransferData->wasNotRunYet(), true);
}

void IoTransferDataTest::singleCheckUnusedDataOnOnConstruct2()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData("", "");
    QCOMPARE(ioTransferData->wasNotRunYet(), true);
}

void IoTransferDataTest::singleCheckUsedDataDataReceived()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData();
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void IoTransferDataTest::singleCheckUsedDataNoAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData("", "", "");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->noAnswerReceived());
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void IoTransferDataTest::singleCheckUsedWrongAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData();
    ioTransferData->m_demoErrorResponse = true;
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->wrongAnswerReceived());
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void IoTransferDataTest::singleCheckUsedPass()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData("fooSend", "fooLead", "fooTrail");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->didIoPass());
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void IoTransferDataTest::groupIdsCreated()
{
    IoTransferDataGroup workGroup1(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    IoTransferDataGroup workGroup2(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    IoTransferDataGroup workGroup3(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);

    QVERIFY(workGroup1.getGroupId() != workGroup2.getGroupId());
    QVERIFY(workGroup2.getGroupId() != workGroup3.getGroupId());
    QVERIFY(workGroup3.getGroupId() != workGroup1.getGroupId());
}

void IoTransferDataTest::groupPassedAllFail()
{
    IoTransferDataGroup::Ptr workTransferGroup =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup->appendTransferList(transList1);

    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData());
    transList2.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup->appendTransferList(transList2);

    // not run communication -> all fail
    // single evaluation is done by io devices
    workTransferGroup->evalAll();
    QCOMPARE(workTransferGroup->passedAll(), false);
}

void IoTransferDataTest::singleDataEvalNotExecuted()
{
    IoTransferDataSingle::Ptr transSingle = IoTransferDataSingleFactory::createIoData("", "");
    QVERIFY(transSingle->wasNotRunYet());
}

void IoTransferDataTest::singleDataEvalNoAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData("", "", "");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->noAnswerReceived());
}

void IoTransferDataTest::singleDataEvalWrongAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData();
    ioTransferData->m_demoErrorResponse = true;
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->wrongAnswerReceived());
}

void IoTransferDataTest::singleDataEvalPass()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingleFactory::createIoData("fooSend", "fooLead", "fooTrail");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QVERIFY(ioTransferData->didIoPass());
}


