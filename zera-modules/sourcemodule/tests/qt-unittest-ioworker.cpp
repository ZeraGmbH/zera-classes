#include "main-unittest-qt.h"
#include "qt-unittest-ioworker.h"
#include "io-interface/iointerfacedemo.h"

static QObject* pIoWorkerTest = addTest(new IoWorkerTest);

void IoWorkerTest::onIoWorkerGroupFinished(IoMultipleTransferGroup workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void IoWorkerTest::init()
{
    m_listIoGroupsReceived.clear();
}

void IoWorkerTest::emptyWorkerIsInvalid()
{
    IoMultipleTransferGroup workGroup;
    QVERIFY(workGroup.m_commandType == COMMAND_UNDEFINED);
    QVERIFY(workGroup.m_errorBehavior == BEHAVE_UNDEFINED);
}

void IoWorkerTest::mulipleTransferIniitialProperties()
{
    IoMultipleTransferGroup workGroup = generateSwitchCommands(false);
    QVERIFY(workGroup.m_commandType != COMMAND_UNDEFINED);
    QVERIFY(workGroup.m_errorBehavior != BEHAVE_UNDEFINED);
}

void IoWorkerTest::noInterfaceNotBusy()
{
    IoMultipleTransferGroup workGroup = generateSwitchCommands(true);
    IoWorker worker;
    worker.enqueueTransferGroup(workGroup);
    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::emptyGroupNotBusy()
{
    IoWorker worker;
    worker.setIoInterface(createOpenInterface());
    IoMultipleTransferGroup workGroup;
    worker.enqueueTransferGroup(workGroup);
    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::notOpenInterfaceNotBusy()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    IoMultipleTransferGroup workGroup = generateSwitchCommands(false);
    IoWorker worker;
    worker.setIoInterface(interface);
    worker.enqueueTransferGroup(workGroup);
    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::openInterfaceBusy()
{
    IoWorker worker;
    IoMultipleTransferGroup workGroup = generateSwitchCommands(false);
    worker.setIoInterface(createOpenInterface());
    worker.enqueueTransferGroup(workGroup);
    QVERIFY(worker.isIoBusy());
}

void IoWorkerTest::noInterfaceNotification()
{
    IoWorker worker;
    IoMultipleTransferGroup workGroup = generateSwitchCommands(true);

    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);
    worker.enqueueTransferGroup(workGroup);
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0] == workGroup); // QCOMPARE on objects does not play well and will be remove in QT6
    evalNotificationCount(0, 0, 0, workGroup.m_ioTransferList.count());
}

void IoWorkerTest::notOpenInterfaceNotifications()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    IoWorker worker;
    IoMultipleTransferGroup workGroup = generateSwitchCommands(true);
    worker.setIoInterface(interface);

    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);
    workGroup.m_groupId = worker.enqueueTransferGroup(workGroup);
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0] == workGroup);
    evalNotificationCount(0, 0, 0, workGroup.m_ioTransferList.count());
}

void IoWorkerTest::disconnectBeforeEnqueue()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);

    demoInterface->simulateExternalDisconnect();
    IoMultipleTransferGroup workGroup = generateSwitchCommands(false);
    worker.enqueueTransferGroup(workGroup);

    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::disconnectWhileWorking()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);
    demoInterface->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoInterface->simulateExternalDisconnect();
    });
    timer.start(10);
    IoMultipleTransferGroup workGroup = generateSwitchCommands(false);
    worker.enqueueTransferGroup(workGroup);
    QTest::qWait(50);

    QVERIFY(!worker.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(0, 0, 0, 1);
}

void IoWorkerTest::disconnectWhileWorkingMultipleNotifications()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);
    demoInterface->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoInterface->simulateExternalDisconnect();
    });
    timer.start(10);
    IoMultipleTransferGroup workGroup1 = generateSwitchCommands(true);
    IoMultipleTransferGroup workGroup2 = generateSwitchCommands(false);
    worker.enqueueTransferGroup(workGroup1);
    worker.enqueueTransferGroup(workGroup2);
    QTest::qWait(50);

    QVERIFY(!worker.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(0, 0, 0, workGroup1.m_ioTransferList.count()+workGroup2.m_ioTransferList.count());
}

using DemoResponseHelper::generateResponseList;

void IoWorkerTest::stopOnFirstError()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 1);
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoMultipleTransferGroup workTransferGroup = generateSwitchCommands(true);
    adjustWorkTransferGroup(workTransferGroup, BEHAVE_STOP_ON_ERROR);

    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResponseList(workTransferGroup);
    responseList[errorIoNumber] = "foo";
    demoInterface->appendResponses(responseList);
    worker.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(100);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0].passedAll(), false);
    // valid data received
    for(int runIo = 0; runIo<errorIoNumber; ++runIo) {
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo].m_dataReceived, responseList[runIo]);
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo].m_IoEval, IOSingleTransferData::EVAL_PASS);
    }
    // invalid data received
    QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[errorIoNumber].m_dataReceived, responseList[errorIoNumber]);
    QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[errorIoNumber].m_IoEval, IOSingleTransferData::EVAL_WRONG_ANSWER);
    // no data received
    for(int notRunIo = errorIoNumber+1; notRunIo<m_listIoGroupsReceived[0].m_ioTransferList.count(); ++notRunIo) {
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[notRunIo].m_dataReceived, "");
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[notRunIo].m_IoEval, IOSingleTransferData::EVAL_NOT_EXECUTED);
    }
}


void IoWorkerTest::continueOnError()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 1);
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoMultipleTransferGroup workTransferGroup = generateSwitchCommands(true);
    adjustWorkTransferGroup(workTransferGroup, BEHAVE_CONTINUE_ON_ERROR);

    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResponseList(workTransferGroup);
    responseList[errorIoNumber] = "foo";
    demoInterface->appendResponses(responseList);
    worker.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(100);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0].passedAll(), false);
    for(int runIo = 0; runIo<m_listIoGroupsReceived[0].m_ioTransferList.count(); ++runIo) {
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo].m_dataReceived, responseList[runIo]);
        if(runIo != errorIoNumber) {
            QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo].m_IoEval, IOSingleTransferData::EVAL_PASS);
        }
        else {
            QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo].m_IoEval, IOSingleTransferData::EVAL_WRONG_ANSWER);
        }
    }
}

void IoWorkerTest::noErrorSigalOnEmptyGroup()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoMultipleTransferGroup workTransferGroup;
    worker.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(10);
    if(m_listIoGroupsReceived.count() != 1) {
        QCOMPARE(m_listIoGroupsReceived.count(), 1);
        QFAIL("Skipping check of error flag");
    }
    QVERIFY(m_listIoGroupsReceived[0].passedAll());
}

void IoWorkerTest::rejectSpam()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    constexpr int maxPendingGroups = 3;
    worker.setMaxPendingGroups(maxPendingGroups);
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoMultipleTransferGroup workTransferGroup = generateSwitchCommands(true);
    adjustWorkTransferGroup(workTransferGroup, BEHAVE_STOP_ON_ERROR);
    QList<QByteArray> responseList = generateResponseList(workTransferGroup);
    for(int group=0; group<maxPendingGroups*2; group++) {
        demoInterface->appendResponses(responseList);
        worker.enqueueTransferGroup(workTransferGroup);
    }
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups*2);
    evalNotificationCount(maxPendingGroups, maxPendingGroups*responseList.count(), 0, maxPendingGroups*responseList.count());
}

void IoWorkerTest::acceptCloseToSpam()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    constexpr int maxPendingGroups = 3;
    worker.setMaxPendingGroups(maxPendingGroups);
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoMultipleTransferGroup workTransferGroup = generateSwitchCommands(true);
    adjustWorkTransferGroup(workTransferGroup, BEHAVE_STOP_ON_ERROR);
    QList<QByteArray> responseList = generateResponseList(workTransferGroup);
    for(int group=0; group<maxPendingGroups; group++) {
        demoInterface->appendResponses(responseList);
        worker.enqueueTransferGroup(workTransferGroup);
    }
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups);
    evalNotificationCount(maxPendingGroups, maxPendingGroups*responseList.count(), 0, 0);
}

void IoWorkerTest::oneValidGroupSingleIo()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoMultipleTransferGroup workTransferGroup = generateSwitchCommands(false);
    QList<QByteArray> responseList = generateResponseList(workTransferGroup);
    for(int group=0; group<workTransferGroup.m_ioTransferList.count(); group++) {
        demoInterface->appendResponses(responseList);
    }
    worker.enqueueTransferGroup(workTransferGroup);
    QVERIFY(worker.isIoBusy());
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, 1, 0, 0);
}

void IoWorkerTest::twoValidGroupsSingleIo()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoMultipleTransferGroup workTransferGroup = generateSwitchCommands(false);
    QList<QByteArray> responseList = generateResponseList(workTransferGroup);
    for(int group=0; group<2*workTransferGroup.m_ioTransferList.count(); group++) {
        demoInterface->appendResponses(responseList);
        worker.enqueueTransferGroup(workTransferGroup);
        QVERIFY(worker.isIoBusy());
    }
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, 2, 0, 0);
}

void IoWorkerTest::oneValidGroupMultipleIo()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoMultipleTransferGroup workTransferGroup = generateSwitchCommands(true);
    int ioCount = workTransferGroup.m_ioTransferList.count();
    demoInterface->appendResponses(generateResponseList(workTransferGroup));

    worker.enqueueTransferGroup(workTransferGroup);
    QVERIFY(worker.isIoBusy());
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, ioCount, 0, 0);
}

void IoWorkerTest::twoValidGroupsMultipleIo()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    int outInCount = 0;
    IoMultipleTransferGroup workTransferGroups[2];
    workTransferGroups[0] = generateSwitchCommands(true);
    workTransferGroups[1] = generateStatusPollCommands();
    int commandCount = 0;
    for(auto &transferGroup: workTransferGroups) {
        outInCount += transferGroup.m_ioTransferList.count();
        demoInterface->appendResponses(generateResponseList(transferGroup));
        worker.enqueueTransferGroup(transferGroup);
        QVERIFY(worker.isIoBusy());
        commandCount++;
    }
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, outInCount, 0, 0);
}

void IoWorkerTest::twoFirstInvalidSecondOkSingleIo()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoMultipleTransferGroup workTransferGroups[2];
    workTransferGroups[0] = generateSwitchCommands(true);
    demoInterface->appendResponses(generateResponseList(workTransferGroups[0]));
    workTransferGroups[1] = generateSwitchCommands(false);
    demoInterface->appendResponses(generateResponseList(workTransferGroups[1]));
    QList<QByteArray>& listResponses = demoInterface->getResponsesForErrorInjection();
    listResponses[0] = "foo";

    worker.enqueueTransferGroup(workTransferGroups[0]);
    QVERIFY(worker.isIoBusy());
    worker.enqueueTransferGroup(workTransferGroups[1]);
    QVERIFY(worker.isIoBusy());

    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(1, 1, 1, 0);
}

void IoWorkerTest::timeoutDetected()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 10);
    IoWorker worker;
    worker.setIoInterface(interface);
    IoMultipleTransferGroup workTransferGroup = generateSwitchCommands(true);
    int finishReceived = 0;
    connect(&worker, &IoWorker::sigTransferGroupFinished, [&](IoMultipleTransferGroup transferGroupToFinish) {
        QVERIFY(transferGroupToFinish.m_ioTransferList.count() > 0);
        // only first I/O is executed
        QCOMPARE(transferGroupToFinish.m_ioTransferList[0].m_IoEval, IOSingleTransferData::EVAL_NO_ANSWER);
        finishReceived++;
    });
    worker.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(30);
    QVERIFY(finishReceived > 0);
}


tIoInterfaceShPtr IoWorkerTest::createOpenInterface()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    return interface;
}

IoMultipleTransferGroup IoWorkerTest::generateStatusPollCommands()
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    return ioGroupGenerator.generateStatusPollGroup();
}

IoMultipleTransferGroup IoWorkerTest::generateSwitchCommands(bool on)
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(on);
    return ioGroupGenerator.generateOnOffGroup(params);
}

void IoWorkerTest::adjustWorkTransferGroup(IoMultipleTransferGroup& workTransferGroup,
                                     GroupErrorBehaviors errorBehavior)
{
    workTransferGroup.m_errorBehavior = errorBehavior;
}

void IoWorkerTest::evalNotificationCount(int passedGroupsExpected, int passExpected, int failExpected, int unknownExpected)
{
    int passedGroupCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    for(int group=0; group<m_listIoGroupsReceived.count(); group++) {
        if(m_listIoGroupsReceived[group].passedAll()) {
            passedGroupCount++;
        }
        for(int io=0; io<m_listIoGroupsReceived[group].m_ioTransferList.count(); ++io) {
            switch(m_listIoGroupsReceived[group].m_ioTransferList[io].m_IoEval) {
            case IOSingleTransferData::EVAL_NOT_EXECUTED:
                unknownCount++;
                break;
            case IOSingleTransferData::EVAL_PASS:
                passCount++;
                break;
            case IOSingleTransferData::EVAL_WRONG_ANSWER:
                failCount++;
                break;
            case IOSingleTransferData::EVAL_NO_ANSWER:
                break;
            }
        }
    }
    QCOMPARE(passedGroupCount, passedGroupsExpected);
    QCOMPARE(passCount, passExpected);
    QCOMPARE(failCount, failExpected);
    QCOMPARE(unknownCount, unknownExpected);
}

