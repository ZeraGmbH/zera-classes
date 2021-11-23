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

void SourceIoWorkerTest::testEmptyWorkerIsInvalid()
{
    cWorkerCommandPacket workPack;
    QVERIFY(workPack.m_commandType == COMMAND_UNDEFINED);
    QVERIFY(workPack.m_errorBehavior == BEHAVE_UNDEFINED);
}

void SourceIoWorkerTest::testCmdPackToWorkType()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_commandType, workPack.m_commandType);
}

void SourceIoWorkerTest::testCmdPackToWorkIoSize()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_outInList.size(), workPack.m_workerIOList.size());
}

void SourceIoWorkerTest::testCmdPackToWorkIoSequence()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    for(int i=0; i<cmdPack.m_outInList.size(); ++i) {
        QCOMPARE(cmdPack.m_outInList[i].m_actionType, workPack.m_workerIOList[i].m_OutIn.m_actionType);
    }
}

void SourceIoWorkerTest::testCmdPackToWorkProperties()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    QVERIFY(workPack.m_commandType != COMMAND_UNDEFINED);
    QVERIFY(workPack.m_errorBehavior != BEHAVE_UNDEFINED);
}

void SourceIoWorkerTest::testNoInterfaceNotBusy()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    cSourceIoWorker worker;
    worker.enqueueAction(workPack);
    QVERIFY(!worker.isBusy());
}

void SourceIoWorkerTest::testEmptyPackNotBusy()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    cWorkerCommandPacket workPack;
    worker.enqueueAction(workPack);
    QVERIFY(!worker.isBusy());
}

static cWorkerCommandPacket generateSwitchCommands(bool on) {
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(on);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    return SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
}

static void enqueueSwitchCommands(cSourceIoWorker& worker, bool on) {
    worker.enqueueAction(generateSwitchCommands(on));
}

static cWorkerCommandPacket generateStatusPollCommands() {
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    cSourceCommandPacket cmdPack = ioPackGenerator.generateStatusPollPacket();
    return SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
}


void SourceIoWorkerTest::testNotOpenInterfaceNotBusy()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    enqueueSwitchCommands(worker, false);
    QVERIFY(!worker.isBusy());
}

static tSourceInterfaceShPtr createOpenDevice() {
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    return interface;
}

void SourceIoWorkerTest::testOpenInterfaceBusy()
{
    cSourceIoWorker worker;
    worker.setIoInterface(createOpenDevice());
    enqueueSwitchCommands(worker, false);
    QVERIFY(worker.isBusy());
}

void SourceIoWorkerTest::testNoInterfaceNotification()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    cSourceIoWorker worker;
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    worker.enqueueAction(workPack);
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QVERIFY(m_listWorkPacksReceived[0] == workPack); // QCOMPARE on objects does not play well and will be remove in QT6
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, 0);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, 0);
    QCOMPARE(unknownCount, cmdPack.m_outInList.count());
}

void SourceIoWorkerTest::testNotOpenInterfaceNotifications()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    workPack.m_workerId = worker.enqueueAction(workPack);
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QVERIFY(m_listWorkPacksReceived[0] == workPack); // QCOMPARE on objects does not play well and will be remove in QT6
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, 0);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, 0);
    QCOMPARE(unknownCount, cmdPack.m_outInList.count());
}

void SourceIoWorkerTest::testDisconnectBeforeEnqueue()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    demoInterface->simulateExternalDisconnect();
    enqueueSwitchCommands(worker, false);
    QVERIFY(!worker.isBusy());
}

void SourceIoWorkerTest::testDisconnectWhileWorking()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
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
    enqueueSwitchCommands(worker, false);
    QTest::qWait(50);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QVERIFY(!worker.isBusy());
    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, 0);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, 0);
    QCOMPARE(unknownCount, 1);
}

void SourceIoWorkerTest::testDisconnectWhileWorkingMultipleNotifications()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
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
    enqueueSwitchCommands(worker, true);
    enqueueSwitchCommands(worker, false);
    enqueueSwitchCommands(worker, true);
    QTest::qWait(50);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QVERIFY(!worker.isBusy());
    QCOMPARE(m_listWorkPacksReceived.count(), 3);
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, 0);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, 0);
    //QCOMPARE(unknownCount, 1);
}

static void adjustWorkCmdPack(cWorkerCommandPacket& workCmdPack,
                                 SourcePacketErrorBehaviors errorBehavior,
                                 SourceIoResponseTypes responseType) {
    workCmdPack.m_errorBehavior = errorBehavior;
    QList<QByteArray> responseList;
    for(auto &io : workCmdPack.m_workerIOList) {
        responseList.append(io.m_OutIn.m_bytesExpected);
        io.m_OutIn.m_responseType = responseType;
    }
}

using SourceDemoHelper::generateResponseList;

void SourceIoWorkerTest::testStopOnFirstErrorFullResponse()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
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
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

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
    tSourceInterfaceShPtr interface = createOpenDevice();
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
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

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
    tSourceInterfaceShPtr interface = createOpenDevice();
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
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

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
    tSourceInterfaceShPtr interface = createOpenDevice();
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
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

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

void SourceIoWorkerTest::countResults(int &actionsPassedCount, int &unknownCount, int &passCount, int &failCount)
{
    for(int pack=0; pack<m_listWorkPacksReceived.count(); pack++) {
        if(m_listWorkPacksReceived[pack].passedAll()) {
            actionsPassedCount++;
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
}

void SourceIoWorkerTest::testSpamRejected()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
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
        int id = worker.enqueueAction(workCmdPack);
        if(pack<maxPendingPackets) {
            QVERIFY(id != 0);
        }
        else {
            QVERIFY(id == 0);
        }
    }
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), maxPendingPackets*2);
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, maxPendingPackets);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, maxPendingPackets*responseList.count());
    QCOMPARE(unknownCount, maxPendingPackets*responseList.count());
}

void SourceIoWorkerTest::testCloseToSpamAccepted()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
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
        int id = worker.enqueueAction(workCmdPack);
        QVERIFY(id != 0);
    }
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), maxPendingPackets);
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, maxPendingPackets);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, maxPendingPackets*responseList.count());
    QCOMPARE(unknownCount, 0);
}

void SourceIoWorkerTest::testOnePacketSingleIoOK()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(false);
    QList<QByteArray> responseList = generateResponseList(workCmdPack, -1);
    for(int pack=0; pack<workCmdPack.m_workerIOList.count(); pack++) {
        demoInterface->appendResponses(responseList);
    }
    int id = worker.enqueueAction(workCmdPack);
    QVERIFY(id != 0);
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, 1);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, 1);
    QCOMPARE(unknownCount, 0);
}

void SourceIoWorkerTest::testTwoPacketSingleIoOK()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(false);
    QList<QByteArray> responseList = generateResponseList(workCmdPack, -1);
    for(int pack=0; pack<2*workCmdPack.m_workerIOList.count(); pack++) {
        demoInterface->appendResponses(responseList);
        int id = worker.enqueueAction(workCmdPack);
        QVERIFY(id != 0);
    }
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 2);
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, 2);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, 2);
    QCOMPARE(unknownCount, 0);
}

void SourceIoWorkerTest::testOnePacketMultipleIoOK()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
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
    int id = worker.enqueueAction(workCmdPack);
    QVERIFY(id != 0);
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, 1);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, ioCount);
    QCOMPARE(unknownCount, 0);
}

void SourceIoWorkerTest::testTwoPacketMultipleIoOK()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
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
        int id = worker.enqueueAction(cmdPack);
        QVERIFY(id != 0);
        commandCount++;
    }
    QTest::qWait(10);
    disconnect(&worker, &cSourceIoWorker::sigCmdFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 2);
    int actionsPassedCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    countResults(actionsPassedCount, unknownCount, passCount, failCount);
    QCOMPARE(actionsPassedCount, 2);
    QCOMPARE(failCount, 0);
    QCOMPARE(passCount, outInCount);
    QCOMPARE(unknownCount, 0);
}

