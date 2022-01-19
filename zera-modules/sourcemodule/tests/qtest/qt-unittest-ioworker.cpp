#include "main-unittest-qt.h"
#include "qt-unittest-ioworker.h"
#include "io-device/iodevicedemo.h"

static QObject* pIoWorkerTest = addTest(new IoWorkerTest);

static void setErrorBehaviorForGroup(IoTransferDataGroup& workTransferGroup,
                                     GroupErrorBehaviors errorBehavior)
{
    workTransferGroup.m_errorBehavior = errorBehavior;
}

void IoWorkerTest::onIoWorkerGroupFinished(IoTransferDataGroup workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void IoWorkerTest::init()
{
    m_listIoGroupsReceived.clear();
}

void IoWorkerTest::emptyWorkerIsInvalid()
{
    IoTransferDataGroup workGroup;
    QVERIFY(workGroup.m_commandType == COMMAND_UNDEFINED);
    QVERIFY(workGroup.m_errorBehavior == BEHAVE_UNDEFINED);
}

void IoWorkerTest::mulipleTransferIniitialProperties()
{
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    QVERIFY(workGroup.m_commandType != COMMAND_UNDEFINED);
    QVERIFY(workGroup.m_errorBehavior != BEHAVE_UNDEFINED);
}

void IoWorkerTest::noInterfaceNotBusy()
{
    IoTransferDataGroup workGroup = generateSwitchCommands(true);
    IoWorker worker;
    worker.enqueueTransferGroup(workGroup);
    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::emptyGroupNotBusy()
{
    IoWorker worker;
    worker.setIoInterface(createOpenInterface());
    IoTransferDataGroup workGroup;
    worker.enqueueTransferGroup(workGroup);
    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::notOpenInterfaceNotBusy()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    IoWorker worker;
    worker.setIoInterface(interface);
    worker.enqueueTransferGroup(workGroup);
    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::openInterfaceBusy()
{
    IoWorker worker;
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    worker.setIoInterface(createOpenInterface());
    worker.enqueueTransferGroup(workGroup);
    QVERIFY(worker.isIoBusy());
}

void IoWorkerTest::noInterfaceNotification()
{
    IoWorker worker;
    IoTransferDataGroup workGroup = generateSwitchCommands(true);

    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);
    worker.enqueueTransferGroup(workGroup);
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0] == workGroup); // QCOMPARE on objects does not play well and will be remove in QT6
    evalNotificationCount(0, 0, 0, workGroup.m_ioTransferList.count());
}

void IoWorkerTest::notOpenInterfaceNotifications()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoWorker worker;
    IoTransferDataGroup workGroup = generateSwitchCommands(true);
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
    tIoDeviceShPtr interface = createOpenInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    IoWorker worker;
    worker.setIoInterface(interface);

    demoInterface->simulateExternalDisconnect();
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    worker.enqueueTransferGroup(workGroup);

    QVERIFY(!worker.isIoBusy());
}

void IoWorkerTest::disconnectWhileWorking()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
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
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    worker.enqueueTransferGroup(workGroup);
    QTest::qWait(50);

    QVERIFY(!worker.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(0, 0, 0, 1);
}

void IoWorkerTest::disconnectWhileWorkingMultipleNotifications()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
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
    IoTransferDataGroup workGroup1 = generateSwitchCommands(true);
    IoTransferDataGroup workGroup2 = generateSwitchCommands(false);
    worker.enqueueTransferGroup(workGroup1);
    worker.enqueueTransferGroup(workGroup2);
    QTest::qWait(50);

    QVERIFY(!worker.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(0, 0, 0, workGroup1.m_ioTransferList.count()+workGroup2.m_ioTransferList.count());
}

void IoWorkerTest::stopOnFirstError()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 1);
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);
    setErrorBehaviorForGroup(workTransferGroup, BEHAVE_STOP_ON_ERROR);

    constexpr int errorIoNumber = 2;
    workTransferGroup.m_ioTransferList[errorIoNumber]->m_demoErrorResponse = true;
    worker.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(100);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0].passedAll(), false);
    // valid data received
    for(int runIo = 0; runIo<errorIoNumber; ++runIo) {
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->m_dataReceived, workTransferGroup.m_ioTransferList[runIo]->getDemoResponse());
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->m_IoEval, IoTransferDataSingle::EVAL_PASS);
    }
    // invalid data received
    QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[errorIoNumber]->m_dataReceived, workTransferGroup.m_ioTransferList[errorIoNumber]->getDemoResponse());
    QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[errorIoNumber]->m_IoEval, IoTransferDataSingle::EVAL_WRONG_ANSWER);
    // no data received
    for(int notRunIo = errorIoNumber+1; notRunIo<m_listIoGroupsReceived[0].m_ioTransferList.count(); ++notRunIo) {
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[notRunIo]->m_dataReceived, "");
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[notRunIo]->m_IoEval, IoTransferDataSingle::EVAL_NOT_EXECUTED);
    }
}

void IoWorkerTest::continueOnError()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 1);
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);
    setErrorBehaviorForGroup(workTransferGroup, BEHAVE_CONTINUE_ON_ERROR);

    constexpr int errorIoNumber = 2;
    workTransferGroup.m_ioTransferList[errorIoNumber]->m_demoErrorResponse = true;

    worker.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(100);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0].passedAll(), false);
    for(int runIo = 0; runIo<m_listIoGroupsReceived[0].m_ioTransferList.count(); ++runIo) {
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->m_dataReceived, m_listIoGroupsReceived[0].m_ioTransferList[runIo]->getDemoResponse());
        if(runIo != errorIoNumber) {
            QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->m_IoEval, IoTransferDataSingle::EVAL_PASS);
        }
        else {
            QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->m_IoEval, IoTransferDataSingle::EVAL_WRONG_ANSWER);
        }
    }
}

void IoWorkerTest::noErrorSigalOnEmptyGroup()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoTransferDataGroup workTransferGroup;
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
    tIoDeviceShPtr interface = createOpenInterface();
    IoWorker worker;
    constexpr int maxPendingGroups = 2;
    worker.setMaxPendingGroups(maxPendingGroups);
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    int countIoPerGroup = 0;
    for(int group=0; group<maxPendingGroups+1; group++) {
        IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);
        countIoPerGroup = workTransferGroup.m_ioTransferList.count();
        setErrorBehaviorForGroup(workTransferGroup, BEHAVE_STOP_ON_ERROR);
        worker.enqueueTransferGroup(workTransferGroup);
    }
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups+1);
    evalNotificationCount(maxPendingGroups, maxPendingGroups*countIoPerGroup, 0, countIoPerGroup);
}

void IoWorkerTest::acceptCloseToSpam()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoWorker worker;
    constexpr int maxPendingGroups = 2;
    worker.setMaxPendingGroups(maxPendingGroups);
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    int countIoPerGroup = 0;
    for(int group=0; group<maxPendingGroups; group++) {
        IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);
        countIoPerGroup = workTransferGroup.m_ioTransferList.count();
        setErrorBehaviorForGroup(workTransferGroup, BEHAVE_STOP_ON_ERROR);
        worker.enqueueTransferGroup(workTransferGroup);
    }
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups);
    evalNotificationCount(maxPendingGroups, maxPendingGroups*countIoPerGroup, 0, 0);
}

void IoWorkerTest::oneValidGroupSingleIo()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoTransferDataGroup workTransferGroup = generateSwitchCommands(false);
    worker.enqueueTransferGroup(workTransferGroup);
    QVERIFY(worker.isIoBusy());
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, 1, 0, 0);
}

void IoWorkerTest::twoValidGroupsSingleIo()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    worker.enqueueTransferGroup(generateSwitchCommands(false));
    worker.enqueueTransferGroup(generateSwitchCommands(false));
    QVERIFY(worker.isIoBusy());

    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, 2, 0, 0);
}

void IoWorkerTest::oneValidGroupMultipleIo()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);

    int ioCount = workTransferGroup.m_ioTransferList.count();
    worker.enqueueTransferGroup(workTransferGroup);
    QVERIFY(worker.isIoBusy());
    QTest::qWait(10);
    disconnect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, ioCount, 0, 0);
}

void IoWorkerTest::twoValidGroupsMultipleIo()
{
    tIoDeviceShPtr interface = createOpenInterface();
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    int outInCount = 0;
    IoTransferDataGroup workTransferGroups[2];
    workTransferGroups[0] = generateSwitchCommands(true);
    workTransferGroups[1] = generateStatusPollCommands();
    int commandCount = 0;
    for(auto &transferGroup: workTransferGroups) {
        outInCount += transferGroup.m_ioTransferList.count();
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
    tIoDeviceShPtr interface = createOpenInterface();
    IoWorker worker;
    worker.setIoInterface(interface);
    connect(&worker, &IoWorker::sigTransferGroupFinished, this, &IoWorkerTest::onIoWorkerGroupFinished);

    IoTransferDataGroup workTransferGroups[2];
    workTransferGroups[0] = generateSwitchCommands(false);
    workTransferGroups[1] = generateSwitchCommands(false);

    workTransferGroups[0].m_ioTransferList[0]->m_demoErrorResponse = true;

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
    tIoDeviceShPtr interface = createOpenInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 10);
    IoWorker worker;
    worker.setIoInterface(interface);

    IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);
    for(auto &ioData : workTransferGroup.m_ioTransferList) {
        ioData->m_bytesExpectedLead.clear();
        ioData->m_bytesExpectedTrail.clear();
    }
    int groupsFinished = 0;
    connect(&worker, &IoWorker::sigTransferGroupFinished, [&](IoTransferDataGroup transferGroupToFinish) {
        QVERIFY(transferGroupToFinish.m_ioTransferList.count() > 0);
        // only first I/O is executed
        QCOMPARE(transferGroupToFinish.m_ioTransferList[0]->m_IoEval, IoTransferDataSingle::EVAL_NO_ANSWER);
        groupsFinished++;
    });

    worker.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(30);

    QCOMPARE(groupsFinished, 1);
}


tIoDeviceShPtr IoWorkerTest::createOpenInterface()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->open(QString());
    return interface;
}

IoTransferDataGroup IoWorkerTest::generateStatusPollCommands()
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    return ioGroupGenerator.generateStatusPollGroup();
}

IoTransferDataGroup IoWorkerTest::generateSwitchCommands(bool on)
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(on);
    return ioGroupGenerator.generateOnOffGroup(params);
}

void IoWorkerTest::evalNotificationCount(int passedGroupsExpected, int passExpected, int failExpected, int unknownExpected)
{
    int passedGroupCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    for(int group=0; group<m_listIoGroupsReceived.count(); group++) {
        if(m_listIoGroupsReceived[group].passedAll()) {
            passedGroupCount++;
        }
        for(int io=0; io<m_listIoGroupsReceived[group].m_ioTransferList.count(); ++io) {
            switch(m_listIoGroupsReceived[group].m_ioTransferList[io]->m_IoEval) {
            case IoTransferDataSingle::EVAL_NOT_EXECUTED:
                unknownCount++;
                break;
            case IoTransferDataSingle::EVAL_PASS:
                passCount++;
                break;
            case IoTransferDataSingle::EVAL_WRONG_ANSWER:
                failCount++;
                break;
            case IoTransferDataSingle::EVAL_NO_ANSWER:
                break;
            }
        }
    }
    QCOMPARE(passedGroupCount, passedGroupsExpected);
    QCOMPARE(passCount, passExpected);
    QCOMPARE(failCount, failExpected);
    QCOMPARE(unknownCount, unknownExpected);
}

