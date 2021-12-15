#include "main-unittest-qt.h"
#include "qt-unittest-sourceioworker.h"

static QObject* pSourceIoWorkerTest = addTest(new SourceIoWorkerTest);

void SourceIoWorkerTest::onWorkPackFinished(cWorkerCommandPacket workPack)
{
    m_listWorkPacksReceived.append(workPack);
}

void SourceIoWorkerTest::init()
{
    m_listWorkPacksReceived.clear();
}

void SourceIoWorkerTest::emptyWorkerIsInvalid()
{
    cWorkerCommandPacket workPack;
    QVERIFY(workPack.m_commandType == COMMAND_UNDEFINED);
    QVERIFY(workPack.m_errorBehavior == BEHAVE_UNDEFINED);
}

void SourceIoWorkerTest::cmdToWorkProperties()
{
    cWorkerCommandPacket workPack = generateSwitchCommands(false);
    QVERIFY(workPack.m_commandType != COMMAND_UNDEFINED);
    QVERIFY(workPack.m_errorBehavior != BEHAVE_UNDEFINED);
}

void SourceIoWorkerTest::cmdPackToWorkIoSize()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    SourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_outInList.size(), workPack.m_workerIOList.size());
}

void SourceIoWorkerTest::cmdPackToWorkIoSequence()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    SourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    for(int i=0; i<cmdPack.m_outInList.size(); ++i) {
        QCOMPARE(cmdPack.m_outInList[i].m_actionType, workPack.m_workerIOList[i].m_OutIn.m_actionType);
    }
}

void SourceIoWorkerTest::noInterfaceNotBusy()
{
    cWorkerCommandPacket workPack = generateSwitchCommands(true);
    cSourceIoWorker worker;
    worker.enqueueAction(workPack);
    QVERIFY(!worker.isIoBusy());
}

void SourceIoWorkerTest::emptyPackNotBusy()
{
    cSourceIoWorker worker;
    worker.setIoInterface(createOpenInterface());
    cWorkerCommandPacket workPack;
    worker.enqueueAction(workPack);
    QVERIFY(!worker.isIoBusy());
}

void SourceIoWorkerTest::notOpenInterfaceNotBusy()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cWorkerCommandPacket workPack = generateSwitchCommands(false);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    worker.enqueueAction(workPack);
    QVERIFY(!worker.isIoBusy());
}

void SourceIoWorkerTest::openInterfaceBusy()
{
    cSourceIoWorker worker;
    cWorkerCommandPacket workPack = generateSwitchCommands(false);
    worker.setIoInterface(createOpenInterface());
    worker.enqueueAction(workPack);
    QVERIFY(worker.isIoBusy());
}

void SourceIoWorkerTest::noInterfaceNotification()
{
    cSourceIoWorker worker;
    cWorkerCommandPacket workPack = generateSwitchCommands(true);

    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    worker.enqueueAction(workPack);
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QVERIFY(m_listWorkPacksReceived[0] == workPack); // QCOMPARE on objects does not play well and will be remove in QT6
    evalNotificationCount(0, 0, 0, workPack.m_workerIOList.count());
}

void SourceIoWorkerTest::notOpenInterfaceNotifications()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cSourceIoWorker worker;
    cWorkerCommandPacket workPack = generateSwitchCommands(true);
    worker.setIoInterface(interface);

    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    workPack.m_workerId = worker.enqueueAction(workPack);
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QVERIFY(m_listWorkPacksReceived[0] == workPack);
    evalNotificationCount(0, 0, 0, workPack.m_workerIOList.count());
}

void SourceIoWorkerTest::disconnectBeforeEnqueue()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);

    demoInterface->simulateExternalDisconnect();
    cWorkerCommandPacket workPack = generateSwitchCommands(false);
    worker.enqueueAction(workPack);

    QVERIFY(!worker.isIoBusy());
}

void SourceIoWorkerTest::disconnectWhileWorking()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    demoInterface->setResponseDelay(500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoInterface->simulateExternalDisconnect();
    });
    timer.start(10);
    cWorkerCommandPacket workPack = generateSwitchCommands(false);
    worker.enqueueAction(workPack);
    QTest::qWait(50);

    QVERIFY(!worker.isIoBusy());
    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    evalNotificationCount(0, 0, 0, 1);
}

void SourceIoWorkerTest::disconnectWhileWorkingMultipleNotifications()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    demoInterface->setResponseDelay(500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoInterface->simulateExternalDisconnect();
    });
    timer.start(10);
    cWorkerCommandPacket workPack1 = generateSwitchCommands(true);
    cWorkerCommandPacket workPack2 = generateSwitchCommands(false);
    worker.enqueueAction(workPack1);
    worker.enqueueAction(workPack2);
    QTest::qWait(50);

    QVERIFY(!worker.isIoBusy());
    QCOMPARE(m_listWorkPacksReceived.count(), 2);
    evalNotificationCount(0, 0, 0, workPack1.m_workerIOList.count()+workPack2.m_workerIOList.count());
}

using SourceDemoHelper::generateResponseList;

void SourceIoWorkerTest::testStopOnFirstErrorFullResponse()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(1);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_STOP_ON_ERROR, RESP_FULL_DATA_SEQUENCE);
    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResponseList(workCmdPack, errorIoNumber);
    demoInterface->setResponses(responseList);
    worker.enqueueAction(workCmdPack);
    QTest::qWait(100);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QCOMPARE(m_listWorkPacksReceived[0].passedAll(), false);
    // valid data received
    for(int runIo = 0; runIo<errorIoNumber; ++runIo) {
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_dataReceived, responseList[runIo]);
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_IoEval, cSourceIoWorkerEntry::EVAL_PASS);
    }
    // invalid data received
    QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[errorIoNumber].m_dataReceived, responseList[errorIoNumber]);
    QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[errorIoNumber].m_IoEval, cSourceIoWorkerEntry::EVAL_FAIL);
    // no data received
    for(int notRunIo = errorIoNumber+1; notRunIo<m_listWorkPacksReceived[0].m_workerIOList.count(); ++notRunIo) {
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[notRunIo].m_dataReceived, "");
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[notRunIo].m_IoEval, cSourceIoWorkerEntry::EVAL_UNKNOWN);
    }
}

void SourceIoWorkerTest::testStopOnFirstErrorPartResponse()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    //demoInterface->setResponseDelay(1);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_STOP_ON_ERROR, RESP_PART_DATA_SEQUENCE);
    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResponseList(workCmdPack, errorIoNumber, "bar");
    demoInterface->setResponses(responseList);
    worker.enqueueAction(workCmdPack);
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QCOMPARE(m_listWorkPacksReceived[0].passedAll(), false);
    // valid data received
    for(int runIo = 0; runIo<errorIoNumber; ++runIo) {
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_dataReceived, responseList[runIo]);
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_IoEval, cSourceIoWorkerEntry::EVAL_PASS);
    }
    // invalid data received
    QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[errorIoNumber].m_dataReceived, responseList[errorIoNumber]);
    QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[errorIoNumber].m_IoEval, cSourceIoWorkerEntry::EVAL_FAIL);
    // no data received
    for(int notRunIo = errorIoNumber+1; notRunIo<m_listWorkPacksReceived[0].m_workerIOList.count(); ++notRunIo) {
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[notRunIo].m_dataReceived, "");
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[notRunIo].m_IoEval, cSourceIoWorkerEntry::EVAL_UNKNOWN);
    }
}

void SourceIoWorkerTest::testContinueOnErrorFullResponse()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(1);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_CONTINUE_ON_ERROR, RESP_FULL_DATA_SEQUENCE);
    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResponseList(workCmdPack, errorIoNumber);
    demoInterface->setResponses(responseList);
    worker.enqueueAction(workCmdPack);
    QTest::qWait(100);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QCOMPARE(m_listWorkPacksReceived[0].passedAll(), false);
    for(int runIo = 0; runIo<m_listWorkPacksReceived[0].m_workerIOList.count(); ++runIo) {
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_dataReceived, responseList[runIo]);
        if(runIo != errorIoNumber) {
            QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_IoEval, cSourceIoWorkerEntry::EVAL_PASS);
        }
        else {
            QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_IoEval, cSourceIoWorkerEntry::EVAL_FAIL);
        }
    }
}

void SourceIoWorkerTest::testContinueOnErrorPartResponse()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    //demoInterface->setResponseDelay(1);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_CONTINUE_ON_ERROR, RESP_PART_DATA_SEQUENCE);
    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResponseList(workCmdPack, errorIoNumber, "bar");
    demoInterface->setResponses(responseList);
    worker.enqueueAction(workCmdPack);
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QCOMPARE(m_listWorkPacksReceived[0].passedAll(), false);
    for(int runIo = 0; runIo<m_listWorkPacksReceived[0].m_workerIOList.count(); ++runIo) {
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_dataReceived, responseList[runIo]);
        if(runIo != errorIoNumber) {
            QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_IoEval, cSourceIoWorkerEntry::EVAL_PASS);
        }
        else {
            QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_IoEval, cSourceIoWorkerEntry::EVAL_FAIL);
        }
    }
}

void SourceIoWorkerTest::testSpamRejected()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    constexpr int maxPendingPackets = 3;
    worker.setMaxPendingActions(maxPendingPackets);
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_STOP_ON_ERROR, RESP_FULL_DATA_SEQUENCE);
    QList<QByteArray> responseList = generateResponseList(workCmdPack, -1);
    for(int pack=0; pack<maxPendingPackets*2; pack++) {
        demoInterface->appendResponses(responseList);
        worker.enqueueAction(workCmdPack);
    }
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), maxPendingPackets*2);
    evalNotificationCount(maxPendingPackets, maxPendingPackets*responseList.count(), 0, maxPendingPackets*responseList.count());
}

void SourceIoWorkerTest::testCloseToSpamAccepted()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    constexpr int maxPendingPackets = 3;
    worker.setMaxPendingActions(maxPendingPackets);
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_STOP_ON_ERROR, RESP_FULL_DATA_SEQUENCE);
    QList<QByteArray> responseList = generateResponseList(workCmdPack, -1);
    for(int pack=0; pack<maxPendingPackets; pack++) {
        demoInterface->appendResponses(responseList);
        worker.enqueueAction(workCmdPack);
    }
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), maxPendingPackets);
    evalNotificationCount(maxPendingPackets, maxPendingPackets*responseList.count(), 0, 0);
}

void SourceIoWorkerTest::testOnePacketSingleIoOK()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(false);
    QList<QByteArray> responseList = generateResponseList(workCmdPack, -1);
    for(int pack=0; pack<workCmdPack.m_workerIOList.count(); pack++) {
        demoInterface->appendResponses(responseList);
    }
    worker.enqueueAction(workCmdPack);
    QVERIFY(worker.isIoBusy());
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    evalNotificationCount(1, 1, 0, 0);
}

void SourceIoWorkerTest::testTwoPacketSingleIoOK()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(false);
    QList<QByteArray> responseList = generateResponseList(workCmdPack, -1);
    for(int pack=0; pack<2*workCmdPack.m_workerIOList.count(); pack++) {
        demoInterface->appendResponses(responseList);
        worker.enqueueAction(workCmdPack);
        QVERIFY(worker.isIoBusy());
    }
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 2);
    evalNotificationCount(2, 2, 0, 0);
}

void SourceIoWorkerTest::testOnePacketMultipleIoOK()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    int ioCount = workCmdPack.m_workerIOList.count();
    QList<QByteArray> responseList = generateResponseList(workCmdPack, -1);
    for(int pack=0; pack<workCmdPack.m_workerIOList.count(); pack++) {
        demoInterface->appendResponses(responseList);
    }
    worker.enqueueAction(workCmdPack);
    QVERIFY(worker.isIoBusy());
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    evalNotificationCount(1, ioCount, 0, 0);
}

void SourceIoWorkerTest::testTwoPacketMultipleIoOK()
{
    tSourceInterfaceShPtr interface = createOpenInterface();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    int outInCount = 0;
    cWorkerCommandPacket workCmdPacks[2];
    workCmdPacks[0] = generateSwitchCommands(true);
    workCmdPacks[1] = generateStatusPollCommands();
    int commandCount = 0;
    QList<QByteArray> responseList;
    for(auto &cmdPack: workCmdPacks) {
        responseList.append(generateResponseList(cmdPack, -1));
        outInCount += cmdPack.m_workerIOList.count();
        for(int pack=0; pack<cmdPack.m_workerIOList.count(); pack++) {
            demoInterface->appendResponses(responseList);
        }
        worker.enqueueAction(cmdPack);
        QVERIFY(worker.isIoBusy());
        commandCount++;
    }
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 2);
    evalNotificationCount(2, outInCount, 0, 0);
}


tSourceInterfaceShPtr SourceIoWorkerTest::createOpenInterface()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    return interface;
}

cWorkerCommandPacket SourceIoWorkerTest::generateStatusPollCommands()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    SourceJsonParamApi params;
    cSourceCommandPacket cmdPack = ioPackGenerator.generateStatusPollPacket();
    return SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
}

cWorkerCommandPacket SourceIoWorkerTest::generateSwitchCommands(bool on)
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    SourceJsonParamApi params;
    params.setOn(on);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    return SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
}

void SourceIoWorkerTest::adjustWorkCmdPack(cWorkerCommandPacket& workCmdPack,
                                 SourcePacketErrorBehaviors errorBehavior,
                                 SourceIoResponseTypes responseType)
{
    workCmdPack.m_errorBehavior = errorBehavior;
    QList<QByteArray> responseList;
    for(auto &io : workCmdPack.m_workerIOList) {
        responseList.append(io.m_OutIn.m_bytesExpected);
        io.m_OutIn.m_responseType = responseType;
    }
}

void SourceIoWorkerTest::evalNotificationCount(int cmdPassedExpected, int passExpected, int failExpected, int unknownExpected)
{
    int cmdPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    for(int pack=0; pack<m_listWorkPacksReceived.count(); pack++) {
        if(m_listWorkPacksReceived[pack].passedAll()) {
            cmdPassedCount++;
        }
        for(int io=0; io<m_listWorkPacksReceived[pack].m_workerIOList.count(); ++io) {
            switch(m_listWorkPacksReceived[pack].m_workerIOList[io].m_IoEval) {
            case cSourceIoWorkerEntry::EVAL_UNKNOWN:
                unknownCount++;
                break;
            case cSourceIoWorkerEntry::EVAL_PASS:
                passCount++;
                break;
            case cSourceIoWorkerEntry::EVAL_FAIL:
                failCount++;
                break;
            }
        }
    }
    QCOMPARE(cmdPassedCount, cmdPassedExpected);
    QCOMPARE(passCount, passExpected);
    QCOMPARE(failCount, failExpected);
    QCOMPARE(unknownCount, unknownExpected);
}

