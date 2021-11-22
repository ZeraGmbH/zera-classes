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
    cWorkerCommandPacket workPack = cSourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_commandType, workPack.m_commandType);
}

void SourceIoWorkerTest::testCmdPackToWorkIoSize()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_singleOutInList.size(), workPack.m_workerIOList.size());
}

void SourceIoWorkerTest::testCmdPackToWorkIoSequence()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    for(int i=0; i<cmdPack.m_singleOutInList.size(); ++i) {
        QCOMPARE(cmdPack.m_singleOutInList[i].m_actionType, workPack.m_workerIOList[i].m_OutIn.m_actionType);
    }
}

void SourceIoWorkerTest::testCmdPackToWorkProperties()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    QVERIFY(workPack.m_commandType != COMMAND_UNDEFINED);
    QVERIFY(workPack.m_errorBehavior != BEHAVE_UNDEFINED);
}

void SourceIoWorkerTest::testNoInterfaceNotBusy()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    cSourceIoWorker worker;
    worker.enqueuePacket(workPack);
    QVERIFY(!worker.isBusy());
}

void SourceIoWorkerTest::testEmptyPackNotBusy()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    cWorkerCommandPacket workPack;
    worker.enqueuePacket(workPack);
    QVERIFY(!worker.isBusy());
}

static cWorkerCommandPacket generateSwitchCommands(bool on) {
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(on);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    return cSourceWorkerConverter::commandPackToWorkerPack(cmdPack);
}

static void enqueueSwitchCommands(cSourceIoWorker& worker, bool on) {
    worker.enqueuePacket(generateSwitchCommands(on));
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
    cWorkerCommandPacket workPack = cSourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    cSourceIoWorker worker;
    connect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    worker.enqueuePacket(workPack);
    QTest::qWait(200);
    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QVERIFY(m_listWorkPacksReceived[0] == workPack); // QCOMPARE on objects does not play well and will be remove in QT6
    disconnect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
}

void SourceIoWorkerTest::testNotOpenInterfaceNotifications()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    workPack.m_workerId = worker.enqueuePacket(workPack);
    QTest::qWait(200);
    QCOMPARE(m_listWorkPacksReceived.count(), 1);
    QVERIFY(m_listWorkPacksReceived[0] == workPack); // QCOMPARE on objects does not play well and will be remove in QT6
    disconnect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
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
    connect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    demoInterface->setResponseDelay(500);
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoInterface->simulateExternalDisconnect();
    });
    timer.start(10);
    enqueueSwitchCommands(worker, false);
    QTest::qWait(200);
    disconnect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    QVERIFY(!worker.isBusy());
    QCOMPARE(m_listWorkPacksReceived.count(), 1);
}

void SourceIoWorkerTest::testDisconnectWhileWorkingMultipleNotifications()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
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
    QTest::qWait(200);
    disconnect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);
    QVERIFY(!worker.isBusy());
    QCOMPARE(m_listWorkPacksReceived.count(), 3);
}

static void adjustWorkCmdPack(cWorkerCommandPacket& workCmdPack,
                                 SourcePacketErrorBehaviors errorBehavior,
                                 SourceResponseTypes responseType) {
    workCmdPack.m_errorBehavior = errorBehavior;
    QList<QByteArray> responseList;
    for(auto &io : workCmdPack.m_workerIOList) {
        responseList.append(io.m_OutIn.m_bytesExpected);
        io.m_OutIn.m_responseType = responseType;
    }
}

static QList<QByteArray> generateResonseList(cWorkerCommandPacket& workCmdPack,
                                             int errorIoNumber,
                                             QByteArray prefix = "") {
    QList<QByteArray> responseList;
    for(auto io : workCmdPack.m_workerIOList) {
        responseList.append(prefix + io.m_OutIn.m_bytesExpected);
    }
    if(errorIoNumber >= 0) {
        responseList[errorIoNumber] = "foo";
    }
    return responseList;
}

void SourceIoWorkerTest::testStopOnFirstErrorFullResponse()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(1);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_STOP_ON_ERROR, RESP_FULL_DATA_SEQUENCE);

    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResonseList(workCmdPack, errorIoNumber);
    demoInterface->setResponses(responseList);

    worker.enqueuePacket(workCmdPack);
    QTest::qWait(300);
    disconnect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
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
    connect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_STOP_ON_ERROR, RESP_PART_DATA_SEQUENCE);

    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResonseList(workCmdPack, errorIoNumber, "bar");
    demoInterface->setResponses(responseList);

    worker.enqueuePacket(workCmdPack);
    QTest::qWait(300);
    disconnect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
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
    connect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_CONTINUE_ON_ERROR, RESP_FULL_DATA_SEQUENCE);

    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResonseList(workCmdPack, errorIoNumber);
    demoInterface->setResponses(responseList);

    worker.enqueuePacket(workCmdPack);
    QTest::qWait(300);
    disconnect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
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
    connect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    cWorkerCommandPacket workCmdPack = generateSwitchCommands(true);
    adjustWorkCmdPack(workCmdPack, BEHAVE_CONTINUE_ON_ERROR, RESP_PART_DATA_SEQUENCE);

    constexpr int errorIoNumber = 2;
    QList<QByteArray> responseList = generateResonseList(workCmdPack, errorIoNumber, "bar");
    demoInterface->setResponses(responseList);

    worker.enqueuePacket(workCmdPack);
    QTest::qWait(300);
    disconnect(&worker, &cSourceIoWorker::sigPackFinished, this, &SourceIoWorkerTest::onWorkPackFinished);

    QCOMPARE(m_listWorkPacksReceived.count(), 1);
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

