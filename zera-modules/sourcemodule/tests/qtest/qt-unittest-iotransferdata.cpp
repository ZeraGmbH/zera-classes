#include "main-unittest-qt.h"
#include "qt-unittest-iotransferdata.h"
#include "io-device/iotransferdatagroup.h"
#include "io-device/iotransferdatasinglefactory.h"
#include "io-device/iodevicebase.h"
#include "sigslotiotransfergroup-forunittest.h"

static QObject* pIoQueueTest = addTest(new IoTransferDataTest);

void IoTransferDataTest::init()
{
}

void IoTransferDataTest::singleDataEvalNotExecutedOnConstruct1()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData();
    QCOMPARE(ioTransferData->getEvaluation(), IoTransferDataSingle::EVAL_NOT_EXECUTED);
}

void IoTransferDataTest::singleDataEvalNotExecutedOnConstruct2()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData("", "");
    QCOMPARE(ioTransferData->getEvaluation(), IoTransferDataSingle::EVAL_NOT_EXECUTED);
}

void IoTransferDataTest::singleDemoResponseLeadTrail()
{
    QByteArray lead = "foo";
    QByteArray trail = "foo";
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData("", lead, trail);
    QCOMPARE(ioTransferData->getDemoResponse(), lead+trail);
}

void IoTransferDataTest::singleDemoResponseLeadTrailEmpty()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData();
    QCOMPARE(ioTransferData->getDemoResponse(), "\r");
}

void IoTransferDataTest::singleDemoResponseSimError()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData();
    ioTransferData->m_demoErrorResponse = true;
    QCOMPARE(ioTransferData->getDemoResponse(), IoTransferDataSingle::demoErrorResponseData);
}

void IoTransferDataTest::singleCheckUnusedDataOnOnConstruct1()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData();
    QCOMPARE(ioTransferData->checkUnusedData(), true);
}

void IoTransferDataTest::singleCheckUnusedDataOnOnConstruct2()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData("", "");
    QCOMPARE(ioTransferData->checkUnusedData(), true);
}

void IoTransferDataTest::singleCheckUsedDataDataReceived()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData();
    tIoDeviceShPtr interface = createOpenDemoInterface();
    interface->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    qputenv("QT_FATAL_CRITICALS", "0");
    QCOMPARE(ioTransferData->checkUnusedData(), false);
    qputenv("QT_FATAL_CRITICALS", "1");
}

void IoTransferDataTest::singleCheckUsedDataNoAnswer()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData("", "", "");
    tIoDeviceShPtr interface = createOpenDemoInterface();
    interface->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QCOMPARE(ioTransferData->getEvaluation(), IoTransferDataSingle::EVAL_NO_ANSWER);
    qputenv("QT_FATAL_CRITICALS", "0");
    QCOMPARE(ioTransferData->checkUnusedData(), false);
    qputenv("QT_FATAL_CRITICALS", "1");
}

void IoTransferDataTest::singleCheckUsedWrongAnswer()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData();
    ioTransferData->m_demoErrorResponse = true;
    tIoDeviceShPtr interface = createOpenDemoInterface();
    interface->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QCOMPARE(ioTransferData->getEvaluation(), IoTransferDataSingle::EVAL_WRONG_ANSWER);
    qputenv("QT_FATAL_CRITICALS", "0");
    QCOMPARE(ioTransferData->checkUnusedData(), false);
    qputenv("QT_FATAL_CRITICALS", "1");
}

void IoTransferDataTest::singleCheckUsedPass()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData("fooSend", "fooLead", "fooTrail");
    tIoDeviceShPtr interface = createOpenDemoInterface();
    interface->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QCOMPARE(ioTransferData->getEvaluation(), IoTransferDataSingle::EVAL_PASS);
    qputenv("QT_FATAL_CRITICALS", "0");
    QCOMPARE(ioTransferData->checkUnusedData(), false);
    qputenv("QT_FATAL_CRITICALS", "1");
}

void IoTransferDataTest::groupEmptyInvalid()
{
    IoTransferDataGroup workGroup;
    QVERIFY(workGroup.getGroupType() == IoTransferDataGroup::GROUP_TYPE_UNDEFINED);
    QVERIFY(workGroup.getErrorBehavior() == IoTransferDataGroup::BEHAVE_UNDEFINED);
}

void IoTransferDataTest::groupIdsCreated()
{
    IoTransferDataGroup workGroup1;
    IoTransferDataGroup workGroup2;
    QVERIFY(workGroup1.getGroupId() != workGroup2.getGroupId());

    IoTransferDataGroup workGroup3(IoTransferDataGroup::GROUP_TYPE_SWITCH_OFF, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    IoTransferDataGroup workGroup4(IoTransferDataGroup::GROUP_TYPE_SWITCH_OFF, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    QVERIFY(workGroup3.getGroupId() != workGroup4.getGroupId());
}

void IoTransferDataTest::groupEqual()
{
    IoTransferDataGroup workTransferGroup1(IoTransferDataGroup::GROUP_TYPE_SWITCH_OFF, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);

    tIoTransferList transList1_1;
    transList1_1.append(IoTransferDataSingleFactory::createIoData("1a","1b", "1c", 1, false));
    transList1_1.append(IoTransferDataSingleFactory::createIoData("2a","2b", "2c", 2, true));
    workTransferGroup1.appendTransferList(transList1_1);

    tIoTransferList transList1_2;
    transList1_2.append(IoTransferDataSingleFactory::createIoData("3a","3b", "3c", 3, false));
    transList1_2.append(IoTransferDataSingleFactory::createIoData("4a","4b", "4c", 4, true));
    workTransferGroup1.appendTransferList(transList1_2);

    IoTransferDataGroup workTransferGroup2 = workTransferGroup1;

    QVERIFY(workTransferGroup1 == workTransferGroup2);
}

void IoTransferDataTest::groupUnequal()
{
    IoTransferDataGroup workTransferGroup1(IoTransferDataGroup::GROUP_TYPE_SWITCH_OFF, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);

    tIoTransferList transList1_1;
    transList1_1.append(IoTransferDataSingleFactory::createIoData("1a","1b", "1c", 1, false));
    transList1_1.append(IoTransferDataSingleFactory::createIoData("2a","2b", "2c", 2, true));
    workTransferGroup1.appendTransferList(transList1_1);

    tIoTransferList transList1_2;
    transList1_2.append(IoTransferDataSingleFactory::createIoData("3a","3b", "3c", 3, false));
    transList1_2.append(IoTransferDataSingleFactory::createIoData("4a","4b", "4c", 4, true));
    workTransferGroup1.appendTransferList(transList1_2);

    IoTransferDataGroup workTransferGroup2 = workTransferGroup1;
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData("3a","3b", "3c", 3, false));
    transList2.append(IoTransferDataSingleFactory::createIoData("4a","4b", "4c", 4, true));
    workTransferGroup1.appendTransferList(transList2);

    QVERIFY(workTransferGroup1 != workTransferGroup2); // yes we could explode this on each (sub)-value...
}

void IoTransferDataTest::groupSignalSlotDirect()
{
    SigSlotIoTransferGroup testTransfer(false);
    QTest::qWait(1);
    QCOMPARE(testTransfer.m_receiveCount, 1);
    QVERIFY(testTransfer.m_groupSend == testTransfer.m_groupReceived);
}

void IoTransferDataTest::groupSignalSlotQueues()
{
    SigSlotIoTransferGroup testTransfer(true);
    QTest::qWait(10);
    QCOMPARE(testTransfer.m_receiveCount, 1);
    QVERIFY(testTransfer.m_groupSend == testTransfer.m_groupReceived);
}

void IoTransferDataTest::groupIsNotSwitchGroup()
{
    IoTransferDataGroup workGroup(IoTransferDataGroup::GROUP_TYPE_STATE_POLL, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    QVERIFY(!workGroup.isSwitchGroup());
}

void IoTransferDataTest::groupIsSwitchGroup()
{
    IoTransferDataGroup workGroup1(IoTransferDataGroup::GROUP_TYPE_SWITCH_ON, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    QVERIFY(workGroup1.isSwitchGroup());
    IoTransferDataGroup workGroup2(IoTransferDataGroup::GROUP_TYPE_SWITCH_OFF, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    QVERIFY(workGroup2.isSwitchGroup());
}

void IoTransferDataTest::groupIsQueryGroup()
{
    IoTransferDataGroup workGroup(IoTransferDataGroup::GROUP_TYPE_STATE_POLL, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    QVERIFY(workGroup.isStateQueryGroup());
}

void IoTransferDataTest::groupIsNotQueryGroup()
{
    IoTransferDataGroup workGroup1(IoTransferDataGroup::GROUP_TYPE_SWITCH_ON, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    QVERIFY(!workGroup1.isStateQueryGroup());
    IoTransferDataGroup workGroup2(IoTransferDataGroup::GROUP_TYPE_SWITCH_OFF, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    QVERIFY(!workGroup2.isStateQueryGroup());
}

void IoTransferDataTest::groupIsUnknownGroup()
{
    IoTransferDataGroup workGroup1(IoTransferDataGroup::GROUP_TYPE_UNDEFINED, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    QVERIFY(!workGroup1.isSwitchGroup());
    QVERIFY(!workGroup1.isStateQueryGroup());
    IoTransferDataGroup workGroup2(IoTransferDataGroup::GROUP_TYPE_UNDEF_BOTTOM, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    QVERIFY(!workGroup1.isSwitchGroup());
    QVERIFY(!workGroup2.isStateQueryGroup());
}

void IoTransferDataTest::groupTypeHasIsMethod()
{
    for(int idx=IoTransferDataGroup::GROUP_TYPE_UNDEFINED+1; idx<IoTransferDataGroup::GROUP_TYPE_UNDEF_BOTTOM; idx++) {
        IoTransferDataGroup workGroup(IoTransferDataGroup::SourceGroupTypes(idx), IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
        QVERIFY(workGroup.isSwitchGroup() || workGroup.isStateQueryGroup());
    }
}

void IoTransferDataTest::singleDataEvalNotExecuted()
{
    tIoTransferDataSingleShPtr transSingle = IoTransferDataSingleFactory::createIoData("", "");
    QCOMPARE(transSingle->getEvaluation(), IoTransferDataSingle::EVAL_NOT_EXECUTED);
}

void IoTransferDataTest::singleDataEvalNoAnswer()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData("", "", "");
    tIoDeviceShPtr interface = createOpenDemoInterface();
    interface->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QCOMPARE(ioTransferData->getEvaluation(), IoTransferDataSingle::EVAL_NO_ANSWER);
}

void IoTransferDataTest::singleDataEvalWrongAnswer()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData();
    ioTransferData->m_demoErrorResponse = true;
    tIoDeviceShPtr interface = createOpenDemoInterface();
    interface->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QCOMPARE(ioTransferData->getEvaluation(), IoTransferDataSingle::EVAL_WRONG_ANSWER);
}

void IoTransferDataTest::singleDataEvalPass()
{
    tIoTransferDataSingleShPtr ioTransferData = IoTransferDataSingleFactory::createIoData("fooSend", "fooLead", "fooTrail");
    tIoDeviceShPtr interface = createOpenDemoInterface();
    interface->sendAndReceive(ioTransferData);
    QTest::qWait(10);
    QCOMPARE(ioTransferData->getEvaluation(), IoTransferDataSingle::EVAL_PASS);
}


