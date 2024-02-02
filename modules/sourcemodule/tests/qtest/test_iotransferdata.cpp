#include "test_iotransferdata.h"
#include "test_globals.h"
#include "ioqueuegroup.h"
#include "iodevicebase.h"

QTEST_MAIN(test_iotransferdata)

void test_iotransferdata::init()
{
}

void test_iotransferdata::singleDataEvalNotExecutedOnConstruct1()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    QVERIFY(ioTransferData->wasNotRunYet());
}

void test_iotransferdata::singleDataEvalNotExecutedOnConstruct2()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", QList<QByteArray>());
    QVERIFY(ioTransferData->wasNotRunYet());
}

void test_iotransferdata::singleDemoResponseLeadTrail()
{
    QByteArray lead = "foo";
    QByteArray trail = "bar";
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", lead, trail);
    QCOMPARE(ioTransferData->getDemoResponder()->getDemoResponse(), lead+trail);
}

void test_iotransferdata::singleDemoResponseLeadTrailEmpty()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    QCOMPARE(ioTransferData->getDemoResponder()->getDemoResponse(), "\r");
}

void test_iotransferdata::singleDemoResponseSimError()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    ioTransferData->getDemoResponder()->activateErrorResponse();
    QCOMPARE(ioTransferData->getDemoResponder()->getDemoResponse(), IoTransferDemoResponder::getDefaultErrorResponse());
}

void test_iotransferdata::singleQueryContent()
{
    QByteArray lead = "foo";
    QByteArray content = "1234";
    QByteArray trail = "bar";
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", lead, trail);
    ioTransferData->setDataReceived(lead+content+trail);
    QCOMPARE(ioTransferData->getBytesQueryContent(), content);
}

void test_iotransferdata::singleQueryContentPass()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "", "");
    ioTransferData->setDataReceived("foo");
    QCOMPARE(ioTransferData->queryContentFailed(), false);
    QCOMPARE(ioTransferData->didIoPass(), true);
}

void test_iotransferdata::singleQueryContentFail()
{
    QByteArray lead = "foo";
    QByteArray content = "1234";
    QByteArray trail = "bar";
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", lead, trail);
    ioTransferData->setCustomQueryContentEvaluator(IIoQueryContentEvaluator::Ptr(new IoQueryContentEvaluatorAlwaysFail));
    ioTransferData->setDataReceived(lead+content+trail);
    QCOMPARE(ioTransferData->queryContentFailed(), true);
    QCOMPARE(ioTransferData->didIoPass(), false);
}

void test_iotransferdata::singleCustomQueryEvaluatorNotSet()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    QCOMPARE(ioTransferData->hasCustomQueryEvaluator(), false);
}

void test_iotransferdata::singleCustomQueryEvaluatorSet()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    ioTransferData->setCustomQueryContentEvaluator(IIoQueryContentEvaluator::Ptr(new IoQueryContentEvaluatorAlwaysFail));
    QCOMPARE(ioTransferData->hasCustomQueryEvaluator(), true);
}

void test_iotransferdata::singleCheckUnusedDataOnOnConstruct1()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    QCOMPARE(ioTransferData->wasNotRunYet(), true);
}

void test_iotransferdata::singleCheckUnusedDataOnOnConstruct2()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", QList<QByteArray>() << "" << "");
    QCOMPARE(ioTransferData->wasNotRunYet(), true);
}

void test_iotransferdata::singleCheckUsedDataDataReceived()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void test_iotransferdata::singleCheckUsedDataNoAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "", "");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData->noAnswerReceived());
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void test_iotransferdata::singleCheckUsedWrongAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "");
    ioTransferData->getDemoResponder()->activateErrorResponse();
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData->wrongAnswerReceived());
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void test_iotransferdata::singleCheckUsedPass()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("fooSend", "fooLead", "fooTrail");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData->didIoPass());
    QCOMPARE(ioTransferData->wasNotRunYet(), false);
}

void test_iotransferdata::singleCheckInjectSingleExpected()
{
    QByteArray lead = "fooLead";
    QByteArray trail = "fooLead";
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", lead, trail);
    QByteArray overrideResponse = lead+"foo"+trail;
    ioTransferData->getDemoResponder()->overrideDefaultResponse(overrideResponse);
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData->didIoPass());
    QCOMPARE(ioTransferData->getBytesReceived(), overrideResponse);
}

void test_iotransferdata::singleCheckInjectMultipleExpected()
{
    QByteArray lead1 = "fooLead1";
    QByteArray lead2 = "fooLead2";
    QByteArray trail = "fooLead";
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();

    IoTransferDataSingle::Ptr ioTransferData1 =
            IoTransferDataSingle::Ptr::create("", QList<QByteArray>()<<lead1<<lead2, trail);
    QByteArray overrideResponse = lead1+"foo"+trail;
    ioTransferData1->getDemoResponder()->overrideDefaultResponse(overrideResponse);
    ioDevice->sendAndReceive(ioTransferData1);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData1->didIoPass());
    QCOMPARE(ioTransferData1->getBytesReceived(), overrideResponse);

    IoTransferDataSingle::Ptr ioTransferData2 =
            IoTransferDataSingle::Ptr::create("", QList<QByteArray>()<<lead1<<lead2, trail);
    overrideResponse = lead2+"foo"+trail;
    ioTransferData2->getDemoResponder()->overrideDefaultResponse(overrideResponse);
    ioDevice->sendAndReceive(ioTransferData2);

    QCoreApplication::processEvents();
    QVERIFY(ioTransferData2->didIoPass());
    QCOMPARE(ioTransferData2->getBytesReceived(), overrideResponse);
}

void test_iotransferdata::groupIdsCreated()
{
    IoQueueGroup workGroup1(IoQueueErrorBehaviors::STOP_ON_ERROR);
    IoQueueGroup workGroup2(IoQueueErrorBehaviors::STOP_ON_ERROR);
    IoQueueGroup workGroup3(IoQueueErrorBehaviors::STOP_ON_ERROR);

    QVERIFY(workGroup1.getGroupId() != workGroup2.getGroupId());
    QVERIFY(workGroup2.getGroupId() != workGroup3.getGroupId());
    QVERIFY(workGroup3.getGroupId() != workGroup1.getGroupId());
}

void test_iotransferdata::groupPassedAllFail()
{
    IoQueueGroup::Ptr workTransferGroup =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList1);

    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList2);

    QCOMPARE(workTransferGroup->passedAll(), false);
}

void test_iotransferdata::singleDataEvalNotExecuted()
{
    IoTransferDataSingle::Ptr transSingle = IoTransferDataSingle::Ptr::create("", "");
    QVERIFY(transSingle->wasNotRunYet());
}

void test_iotransferdata::singleDataEvalNoAnswer()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("", "", "");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData->noAnswerReceived());
}

void test_iotransferdata::singleDataEvalWrongAnswerConstruct1()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("send", "lead", "trail");
    ioTransferData->getDemoResponder()->activateErrorResponse();
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData->wrongAnswerReceived());
}

void test_iotransferdata::singleDataEvalWrongAnswerConstruct2()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("send",
                                                                                 QList<QByteArray>() << "lead1" << "lead2",
                                                                                 "trail");
    ioTransferData->getDemoResponder()->activateErrorResponse();
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData->wrongAnswerReceived());
}

void test_iotransferdata::singleDataEvalPassConstruct1()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("fooSend", "fooLead", "fooTrail");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData->didIoPass());
}

void test_iotransferdata::singleDataEvalPassConstruct2()
{
    IoTransferDataSingle::Ptr ioTransferData = IoTransferDataSingle::Ptr::create("fooSend",
                                                                                 QList<QByteArray>() << "fooLead1" << "fooLead2",
                                                                                 "fooTrail");
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ioDevice->sendAndReceive(ioTransferData);
    QCoreApplication::processEvents();
    QVERIFY(ioTransferData->didIoPass());
}


