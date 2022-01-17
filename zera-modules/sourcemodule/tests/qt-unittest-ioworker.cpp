#include "main-unittest-qt.h"
#include "qt-unittest-ioworker.h"
#include "io-interface/iointerfacedemo.h"

static QObject* pIoWorkerTest = addTest(new IoWorkerTest);

void IoWorkerTest::onWorkPackFinished(IoWorkerCmdPack workPack)
{
    m_listWorkPacksReceived.append(workPack);
}

void IoWorkerTest::init()
{
    m_listWorkPacksReceived.clear();
}

void IoWorkerTest::emptyWorkerIsInvalid()
{
    IoWorkerCmdPack workPack;
    QVERIFY(workPack.m_commandType == COMMAND_UNDEFINED);
    QVERIFY(workPack.m_errorBehavior == BEHAVE_UNDEFINED);
}

void IoWorkerTest::cmdToWorkProperties()
{
    IoWorkerCmdPack workPack = generateSwitchCommands(false);
    QVERIFY(workPack.m_commandType != COMMAND_UNDEFINED);
    QVERIFY(workPack.m_errorBehavior != BEHAVE_UNDEFINED);
}

void IoWorkerTest::cmdPackToWorkIoSize()
{
    IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(true);
    IoCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    IoWorkerCmdPack workPack = IoWorkerConverter::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_outInList.size(), workPack.m_workerIOList.size());
}

void IoWorkerTest::cmdPackToWorkIoSequence()
{
    IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(true);
    IoCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    IoWorkerCmdPack workPack = IoWorkerConverter::commandPackToWorkerPack(cmdPack);
    for(int i=0; i<cmdPack.m_outInList.size(); ++i) {
        QCOMPARE(cmdPack.m_outInList[i].m_actionType, workPack.m_workerIOList[i].m_OutIn.m_actionType);
    }
}

void IoWorkerTest::noInterfaceNotBusy()
{
    IoWorkerCmdPack workPack = generateSwitchCommands(true);
    IoWorker worker;
    worker.enqueueAction(workPack);
    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::emptyPackNotBusy()
{
    IoWorker worker;
    worker.setIoInterface(createOpenInterface());
    IoWorkerCmdPack workPack;
    worker.enqueueAction(workPack);
    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::notOpenInterfaceNotBusy()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    IoWorkerCmdPack workPack = generateSwitchCommands(false);
    IoWorker worker;
    worker.setIoInterface(interface);
    worker.enqueueAction(workPack);
    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::openInterfaceBusy()
{
    IoWorker worker;
    IoWorkerCmdPack workPack = generateSwitchCommands(false);
    worker.setIoInterface(createOpenInterface());
    worker.enqueueAction(workPack);
    QVERIFY(worker.isIoBusy());
}

void IoWorkerTest::noInterfaceNotification()
{
    IoWorker worker;
    IoWorkerCmdPack workPack = generateSwitchCommands(true);

    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);
    worker.enqueueAction(workPack);
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QVERIFY(m_listWorkPacksReceived[0] == workPack); // QCOMPARE on objects does not play well and will be remove in QT6
    evalNotificationCount(0, 0, 0, workPack.m_workerIOList.count());
}

void IoWorkerTest::notOpenInterfaceNotifications()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    IoWorker worker;
    IoWorkerCmdPack workPack = generateSwitchCommands(true);
    worker.setIoInterface(interface);

    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);
    workPack.m_workerId = worker.enqueueAction(workPack);
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QVERIFY(m_listWorkPacksReceived[0] == workPack);
    evalNotificationCount(0, 0, 0, workPack.m_workerIOList.count());
}

void IoWorkerTest::disconnectBeforeEnqueue()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);

    demoInterface->simulateExternalDisconnect();
    IoWorkerCmdPack workPack = generateSwitchCommands(false);
    worker.enqueueAction(workPack);

    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::disconnectWhileWorking()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);
    demoInterface->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoInterface->simulateExternalDisconnect();
    });
    timer.start(10);
    IoWorkerCmdPack workPack = generateSwitchCommands(false);
    worker.enqueueAction(workPack);
    QTest::qWait(50);

    QVERIFY(!worker.isIoBusy());
    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    evalNotificationCount(0, 0, 0, 1);
}

void IoWorkerTest::disconnectWhileWorkingMultipleNotifications()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);
    demoInterface->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoInterface->simulateExternalDisconnect();
    });
    timer.start(10);
    IoWorkerCmdPack workPack1 = generateSwitchCommands(true);
    IoWorkerCmdPack workPack2 = generateSwitchCommands(false);
    worker.enqueueAction(workPack1);
    worker.enqueueAction(workPack2);
    QTest::qWait(50);

    QVERIFY(!worker.isIoBusy());
    QCOMPARE(m_listWorkPacksReceived.count(), 2);
    evalNotificationCount(0, 0, 0, workPack1.m_workerIOList.count()+workPack2.m_workerIOList.count());
}

using DemoResponseHelper::generateResponseList;

void IoWorkerTest::testStopOnFirstError()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 1);
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    IoWorkerCmdPack workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_STOP_ON_ERROR);

    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResponseList(workCmdPack);
    responseList[errorIoNumber] = "foo";
    demoInterface->appendResponses(responseList);
    worker.enqueueAction(workCmdPack);
    QTest::qWait(100);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QCOMPARE(m_listWorkPacksReceived[0].passedAll(), false);
    // valid data received
    for(int runIo = 0; runIo<errorIoNumber; ++runIo) {
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_dataReceived, responseList[runIo]);
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_IoEval, IoWorkerEntry::EVAL_PASS);
    }
    // invalid data received
    QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[errorIoNumber].m_dataReceived, responseList[errorIoNumber]);
    QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[errorIoNumber].m_IoEval, IoWorkerEntry::EVAL_WRONG_ANSWER);
    // no data received
    for(int notRunIo = errorIoNumber+1; notRunIo<m_listWorkPacksReceived[0].m_workerIOList.count(); ++notRunIo) {
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[notRunIo].m_dataReceived, "");
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[notRunIo].m_IoEval, IoWorkerEntry::EVAL_UNKNOWN);
    }
}


void IoWorkerTest::testContinueOnError()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 1);
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    IoWorkerCmdPack workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_CONTINUE_ON_ERROR);

    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResponseList(workCmdPack);
    responseList[errorIoNumber] = "foo";
    demoInterface->appendResponses(responseList);
    worker.enqueueAction(workCmdPack);
    QTest::qWait(100);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QCOMPARE(m_listWorkPacksReceived[0].passedAll(), false);
    for(int runIo = 0; runIo<m_listWorkPacksReceived[0].m_workerIOList.count(); ++runIo) {
        QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_dataReceived, responseList[runIo]);
        if(runIo != errorIoNumber) {
            QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_IoEval, IoWorkerEntry::EVAL_PASS);
        }
        else {
            QCOMPARE(m_listWorkPacksReceived[0].m_workerIOList[runIo].m_IoEval, IoWorkerEntry::EVAL_WRONG_ANSWER);
        }
    }
}

void IoWorkerTest::noErrorSigOnEmptyPack()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    IoWorkerCmdPack workCmdPack;
    worker.enqueueAction(workCmdPack);
    QTest::qWait(10);
    if(m_listWorkPacksReceived.count() != 1) {
        QCOMPARE(m_listWorkPacksReceived.count(), 1);
        QFAIL("Skipping check of error flag");
    }
    QVERIFY(m_listWorkPacksReceived[0].passedAll());
}

void IoWorkerTest::testSpamRejected()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    constexpr int maxPendingPackets = 3;
    worker.setMaxPendingActions(maxPendingPackets);
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    IoWorkerCmdPack workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_STOP_ON_ERROR);
    QList<QByteArray> responseList = generateResponseList(workCmdPack);
    for(int pack=0; pack<maxPendingPackets*2; pack++) {
        demoInterface->appendResponses(responseList);
        worker.enqueueAction(workCmdPack);
    }
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), maxPendingPackets*2);
    evalNotificationCount(maxPendingPackets, maxPendingPackets*responseList.count(), 0, maxPendingPackets*responseList.count());
}

void IoWorkerTest::testCloseToSpamAccepted()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    constexpr int maxPendingPackets = 3;
    worker.setMaxPendingActions(maxPendingPackets);
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    IoWorkerCmdPack workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_STOP_ON_ERROR);
    QList<QByteArray> responseList = generateResponseList(workCmdPack);
    for(int pack=0; pack<maxPendingPackets; pack++) {
        demoInterface->appendResponses(responseList);
        worker.enqueueAction(workCmdPack);
    }
    QTest::qWait(10);

    QCOMPARE(m_listWorkPacksReceived.count(), maxPendingPackets);
    evalNotificationCount(maxPendingPackets, maxPendingPackets*responseList.count(), 0, 0);
}

void IoWorkerTest::testOnePacketSingleIoOK()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    IoWorkerCmdPack workCmdPack = generateSwitchCommands(false);
    QList<QByteArray> responseList = generateResponseList(workCmdPack);
    for(int pack=0; pack<workCmdPack.m_workerIOList.count(); pack++) {
        demoInterface->appendResponses(responseList);
    }
    worker.enqueueAction(workCmdPack);
    QVERIFY(worker.isIoBusy());
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    evalNotificationCount(1, 1, 0, 0);
}

void IoWorkerTest::testTwoPacketSingleIoOK()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    IoWorkerCmdPack workCmdPack = generateSwitchCommands(false);
    QList<QByteArray> responseList = generateResponseList(workCmdPack);
    for(int pack=0; pack<2*workCmdPack.m_workerIOList.count(); pack++) {
        demoInterface->appendResponses(responseList);
        worker.enqueueAction(workCmdPack);
        QVERIFY(worker.isIoBusy());
    }
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 2);
    evalNotificationCount(2, 2, 0, 0);
}

void IoWorkerTest::testOnePacketMultipleIoOK()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    IoWorkerCmdPack workCmdPack = generateSwitchCommands(true);
    int ioCount = workCmdPack.m_workerIOList.count();
    QList<QByteArray> responseList = generateResponseList(workCmdPack);
    for(int pack=0; pack<workCmdPack.m_workerIOList.count(); pack++) {
        demoInterface->appendResponses(responseList);
    }
    worker.enqueueAction(workCmdPack);
    QVERIFY(worker.isIoBusy());
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    evalNotificationCount(1, ioCount, 0, 0);
}

void IoWorkerTest::testTwoPacketMultipleIoOK()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    int outInCount = 0;
    IoWorkerCmdPack workCmdPacks[2];
    workCmdPacks[0] = generateSwitchCommands(true);
    workCmdPacks[1] = generateStatusPollCommands();
    int commandCount = 0;
    for(auto &cmdPack: workCmdPacks) {
        QList<QByteArray> responseList;
        responseList.append(generateResponseList(cmdPack));
        outInCount += cmdPack.m_workerIOList.count();
        demoInterface->appendResponses(responseList);
        worker.enqueueAction(cmdPack);
        QVERIFY(worker.isIoBusy());
        commandCount++;
    }
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigCmdFinished, this, &IoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 2);
    evalNotificationCount(2, outInCount, 0, 0);
}

void IoWorkerTest::timeoutDetected()
{
    tIoInterfaceShPtr interface = createOpenInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 10);
    IoWorker worker;
    worker.setIoInterface(interface);
    IoWorkerCmdPack workCmdPack = generateSwitchCommands(true);
    int finishReceived = 0;
    connect(&worker, &IoWorker::sigCmdFinished, [&](IoWorkerCmdPack cmdToFinish) {
        QVERIFY(cmdToFinish.m_workerIOList.count() > 0);
        // only first I/O is executed
        QCOMPARE(cmdToFinish.m_workerIOList[0].m_IoEval, IoWorkerEntry::EVAL_NO_ANSWER);
        finishReceived++;
    });
    worker.enqueueAction(workCmdPack);
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

IoWorkerCmdPack IoWorkerTest::generateStatusPollCommands()
{
    IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
    JsonParamApi params;
    IoCommandPacket cmdPack = ioPackGenerator.generateStatusPollPacket();
    return IoWorkerConverter::commandPackToWorkerPack(cmdPack);
}

IoWorkerCmdPack IoWorkerTest::generateSwitchCommands(bool on)
{
    IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(on);
    IoCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    return IoWorkerConverter::commandPackToWorkerPack(cmdPack);
}

void IoWorkerTest::adjustWorkCmdPack(IoWorkerCmdPack& workCmdPack,
                                     PacketErrorBehaviors errorBehavior)
{
    workCmdPack.m_errorBehavior = errorBehavior;
}

void IoWorkerTest::evalNotificationCount(int cmdPassedExpected, int passExpected, int failExpected, int unknownExpected)
{
    int cmdPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    for(int pack=0; pack<m_listWorkPacksReceived.count(); pack++) {
        if(m_listWorkPacksReceived[pack].passedAll()) {
            cmdPassedCount++;
        }
        for(int io=0; io<m_listWorkPacksReceived[pack].m_workerIOList.count(); ++io) {
            switch(m_listWorkPacksReceived[pack].m_workerIOList[io].m_IoEval) {
            case IoWorkerEntry::EVAL_UNKNOWN:
                unknownCount++;
                break;
            case IoWorkerEntry::EVAL_PASS:
                passCount++;
                break;
            case IoWorkerEntry::EVAL_WRONG_ANSWER:
                failCount++;
                break;
            // timeout has an explicit test
            case IoWorkerEntry::EVAL_NO_ANSWER:
                break;
            }
        }
    }
    QCOMPARE(cmdPassedCount, cmdPassedExpected);
    QCOMPARE(passCount, passExpected);
    QCOMPARE(failCount, failExpected);
    QCOMPARE(unknownCount, unknownExpected);
}

