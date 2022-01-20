#include "main-unittest-qt.h"
#include "qt-unittest-ioqueue.h"
#include "io-device/iodevicedemo.h"

static QObject* pIoQueueTest = addTest(new IoQueueTest);

static void setErrorBehaviorForGroup(IoTransferDataGroup& workTransferGroup,
                                     GroupErrorBehaviors errorBehavior)
{
    workTransferGroup.m_errorBehavior = errorBehavior;
}

void IoQueueTest::onIoQueueGroupFinished(IoTransferDataGroup workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void IoQueueTest::init()
{
    m_listIoGroupsReceived.clear();
}

void IoQueueTest::emptyQueueIsInvalid()
{
    IoTransferDataGroup workGroup;
    QVERIFY(workGroup.m_commandType == COMMAND_UNDEFINED);
    QVERIFY(workGroup.m_errorBehavior == BEHAVE_UNDEFINED);
}

void IoQueueTest::mulipleTransferIniitialProperties()
{
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    QVERIFY(workGroup.m_commandType != COMMAND_UNDEFINED);
    QVERIFY(workGroup.m_errorBehavior != BEHAVE_UNDEFINED);
}

void IoQueueTest::noInterfaceNotBusy()
{
    IoTransferDataGroup workGroup = generateSwitchCommands(true);
    IoQueue queue;
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void IoQueueTest::emptyGroupNotBusy()
{
    IoQueue queue;
    queue.setIoInterface(createOpenDemoInterface());
    IoTransferDataGroup workGroup;
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void IoQueueTest::notOpenInterfaceNotBusy()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    IoQueue queue;
    queue.setIoInterface(interface);
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void IoQueueTest::openInterfaceBusy()
{
    IoQueue queue;
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    queue.setIoInterface(createOpenDemoInterface());
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(queue.isIoBusy());
}

void IoQueueTest::noInterfaceNotification()
{
    IoQueue queue;
    IoTransferDataGroup workGroup = generateSwitchCommands(true);

    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);
    queue.enqueueTransferGroup(workGroup);
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0] == workGroup); // QCOMPARE on objects does not play well and will be remove in QT6
    evalNotificationCount(0, 0, 0, workGroup.m_ioTransferList.count());
}

void IoQueueTest::notOpenInterfaceNotifications()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoQueue queue;
    IoTransferDataGroup workGroup = generateSwitchCommands(true);
    queue.setIoInterface(interface);

    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);
    workGroup.m_groupId = queue.enqueueTransferGroup(workGroup);
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0] == workGroup);
    evalNotificationCount(0, 0, 0, workGroup.m_ioTransferList.count());
}

void IoQueueTest::disconnectBeforeEnqueue()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    IoQueue queue;
    queue.setIoInterface(interface);

    demoInterface->simulateExternalDisconnect();
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup);

    QVERIFY(!queue.isIoBusy());
}

void IoQueueTest::disconnectWhileWorking()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);
    demoInterface->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoInterface->simulateExternalDisconnect();
    });
    timer.start(10);
    IoTransferDataGroup workGroup = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup);
    QTest::qWait(50);

    QVERIFY(!queue.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(0, 0, 0, 1);
}

void IoQueueTest::disconnectWhileWorkingMultipleNotifications()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);
    demoInterface->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoInterface->simulateExternalDisconnect();
    });
    timer.start(10);
    IoTransferDataGroup workGroup1 = generateSwitchCommands(true);
    IoTransferDataGroup workGroup2 = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup1);
    queue.enqueueTransferGroup(workGroup2);
    QTest::qWait(50);

    QVERIFY(!queue.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(0, 0, 0, workGroup1.m_ioTransferList.count()+workGroup2.m_ioTransferList.count());
}

void IoQueueTest::stopOnFirstError()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 1);
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);
    setErrorBehaviorForGroup(workTransferGroup, BEHAVE_STOP_ON_ERROR);

    constexpr int errorIoNumber = 2;
    workTransferGroup.m_ioTransferList[errorIoNumber]->m_demoErrorResponse = true;
    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(100);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0].passedAll(), false);
    // valid data received
    for(int runIo = 0; runIo<errorIoNumber; ++runIo) {
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->getDataReceived(), workTransferGroup.m_ioTransferList[runIo]->getDemoResponse());
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->getEvaluation(), IoTransferDataSingle::EVAL_PASS);
    }
    // invalid data received
    QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[errorIoNumber]->getDataReceived(), workTransferGroup.m_ioTransferList[errorIoNumber]->getDemoResponse());
    QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[errorIoNumber]->getEvaluation(), IoTransferDataSingle::EVAL_WRONG_ANSWER);
    // no data received
    for(int notRunIo = errorIoNumber+1; notRunIo<m_listIoGroupsReceived[0].m_ioTransferList.count(); ++notRunIo) {
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[notRunIo]->getDataReceived(), "");
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[notRunIo]->getEvaluation(), IoTransferDataSingle::EVAL_NOT_EXECUTED);
    }
}

void IoQueueTest::continueOnError()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 1);
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);
    setErrorBehaviorForGroup(workTransferGroup, BEHAVE_CONTINUE_ON_ERROR);

    constexpr int errorIoNumber = 2;
    workTransferGroup.m_ioTransferList[errorIoNumber]->m_demoErrorResponse = true;

    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(100);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0].passedAll(), false);
    for(int runIo = 0; runIo<m_listIoGroupsReceived[0].m_ioTransferList.count(); ++runIo) {
        QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->getDataReceived(), m_listIoGroupsReceived[0].m_ioTransferList[runIo]->getDemoResponse());
        if(runIo != errorIoNumber) {
            QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->getEvaluation(), IoTransferDataSingle::EVAL_PASS);
        }
        else {
            QCOMPARE(m_listIoGroupsReceived[0].m_ioTransferList[runIo]->getEvaluation(), IoTransferDataSingle::EVAL_WRONG_ANSWER);
        }
    }
}

void IoQueueTest::noErrorSigalOnEmptyGroup()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup workTransferGroup;
    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(10);
    if(m_listIoGroupsReceived.count() != 1) {
        QCOMPARE(m_listIoGroupsReceived.count(), 1);
        QFAIL("Skipping check of error flag");
    }
    QVERIFY(m_listIoGroupsReceived[0].passedAll());
}

void IoQueueTest::rejectSpam()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoQueue queue;
    constexpr int maxPendingGroups = 2;
    queue.setMaxPendingGroups(maxPendingGroups);
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    int countIoPerGroup = 0;
    for(int group=0; group<maxPendingGroups+1; group++) {
        IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);
        countIoPerGroup = workTransferGroup.m_ioTransferList.count();
        setErrorBehaviorForGroup(workTransferGroup, BEHAVE_STOP_ON_ERROR);
        queue.enqueueTransferGroup(workTransferGroup);
    }
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups+1);
    evalNotificationCount(maxPendingGroups, maxPendingGroups*countIoPerGroup, 0, countIoPerGroup);
}

void IoQueueTest::acceptCloseToSpam()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoQueue queue;
    constexpr int maxPendingGroups = 2;
    queue.setMaxPendingGroups(maxPendingGroups);
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    int countIoPerGroup = 0;
    for(int group=0; group<maxPendingGroups; group++) {
        IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);
        countIoPerGroup = workTransferGroup.m_ioTransferList.count();
        setErrorBehaviorForGroup(workTransferGroup, BEHAVE_STOP_ON_ERROR);
        queue.enqueueTransferGroup(workTransferGroup);
    }
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups);
    evalNotificationCount(maxPendingGroups, maxPendingGroups*countIoPerGroup, 0, 0);
}

void IoQueueTest::oneValidGroupSingleIo()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup workTransferGroup = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workTransferGroup);
    QVERIFY(queue.isIoBusy());
    QTest::qWait(10);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, 1, 0, 0);
}

void IoQueueTest::twoValidGroupsSingleIo()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    queue.enqueueTransferGroup(generateSwitchCommands(false));
    queue.enqueueTransferGroup(generateSwitchCommands(false));
    QVERIFY(queue.isIoBusy());

    QTest::qWait(10);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, 2, 0, 0);
}

void IoQueueTest::oneValidGroupMultipleIo()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup workTransferGroup = generateSwitchCommands(true);

    int ioCount = workTransferGroup.m_ioTransferList.count();
    queue.enqueueTransferGroup(workTransferGroup);
    QVERIFY(queue.isIoBusy());
    QTest::qWait(10);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, ioCount, 0, 0);
}

void IoQueueTest::twoValidGroupsMultipleIo()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    int outInCount = 0;
    IoTransferDataGroup workTransferGroups[2];
    workTransferGroups[0] = generateSwitchCommands(true);
    workTransferGroups[1] = generateStatusPollCommands();
    int commandCount = 0;
    for(auto &transferGroup: workTransferGroups) {
        outInCount += transferGroup.m_ioTransferList.count();
        queue.enqueueTransferGroup(transferGroup);
        QVERIFY(queue.isIoBusy());
        commandCount++;
    }
    QTest::qWait(10);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, outInCount, 0, 0);
}

void IoQueueTest::twoFirstInvalidSecondOkSingleIo()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoInterface(interface);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup workTransferGroups[2];
    workTransferGroups[0] = generateSwitchCommands(false);
    workTransferGroups[1] = generateSwitchCommands(false);

    workTransferGroups[0].m_ioTransferList[0]->m_demoErrorResponse = true;

    queue.enqueueTransferGroup(workTransferGroups[0]);
    QVERIFY(queue.isIoBusy());
    queue.enqueueTransferGroup(workTransferGroups[1]);
    QVERIFY(queue.isIoBusy());

    QTest::qWait(10);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(1, 1, 1, 0);
}

IoTransferDataGroup IoQueueTest::generateStatusPollCommands()
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    return ioGroupGenerator.generateStatusPollGroup();
}

IoTransferDataGroup IoQueueTest::generateSwitchCommands(bool on)
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(on);
    return ioGroupGenerator.generateOnOffGroup(params);
}

void IoQueueTest::evalNotificationCount(int passedGroupsExpected, int passExpected, int failExpected, int unknownExpected)
{
    int passedGroupCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    for(int group=0; group<m_listIoGroupsReceived.count(); group++) {
        if(m_listIoGroupsReceived[group].passedAll()) {
            passedGroupCount++;
        }
        for(int io=0; io<m_listIoGroupsReceived[group].m_ioTransferList.count(); ++io) {
            switch(m_listIoGroupsReceived[group].m_ioTransferList[io]->getEvaluation()) {
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

