#include "main-unittest-qt.h"
#include "qt-unittest-ioqueue.h"
#include "io-device/iodevicefactory.h"
#include "io-device/iodevicedemo.h"
#include "io-device/iotransferdatasinglefactory.h"
#include "source-protocols/iogroupgenerator.h"

static QObject* pIoQueueTest = addTest(new IoQueueTest);

void IoQueueTest::onIoQueueGroupFinished(IoTransferDataGroup::Ptr workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void IoQueueTest::init()
{
    m_listIoGroupsReceived.clear();
}

void IoQueueTest::noIoDeviceNotBusy()
{
    IoTransferDataGroup::Ptr workGroup = generateSwitchCommands(true);
    IoQueue queue;
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void IoQueueTest::emptyGroupNotBusy()
{
    IoQueue queue;
    queue.setIoDevice(createOpenDemoInterface());
    IoTransferDataGroup::Ptr workGroup = IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void IoQueueTest::nullGroupNotBusy()
{
    IoQueue queue;
    queue.setIoDevice(createOpenDemoInterface());
    IoTransferDataGroup::Ptr null;
    queue.enqueueTransferGroup(null);
    QVERIFY(!queue.isIoBusy());
}

void IoQueueTest::notOpenIoDeviceNotBusy()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    IoTransferDataGroup::Ptr workGroup = generateSwitchCommands(false);
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void IoQueueTest::openIoDeviceBusy()
{
    IoQueue queue;
    IoTransferDataGroup::Ptr workGroup = generateSwitchCommands(false);
    queue.setIoDevice(createOpenDemoInterface());
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(queue.isIoBusy());
}

void IoQueueTest::noIoDeviceNotification()
{
    IoQueue queue;
    IoTransferDataGroup::Ptr workGroup = generateSwitchCommands(true);

    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);
    queue.enqueueTransferGroup(workGroup);
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0].get() == workGroup.get());
    evalNotificationCount(0, 0, 0, workGroup->getTransferCount());
}

void IoQueueTest::notOpenIoDeviceNotifications()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    IoQueue queue;
    IoTransferDataGroup::Ptr workGroup = generateSwitchCommands(true);
    queue.setIoDevice(ioDevice);

    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);
    queue.enqueueTransferGroup(workGroup);
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0] == workGroup);
    evalNotificationCount(0, 0, 0, workGroup->getTransferCount());
}

void IoQueueTest::disconnectBeforeEnqueue()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    IoQueue queue;
    queue.setIoDevice(ioDevice);

    demoIoDevice->simulateExternalDisconnect();
    IoTransferDataGroup::Ptr workGroup = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup);

    QVERIFY(!queue.isIoBusy());
}

void IoQueueTest::disconnectWhileWorking()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);
    demoIoDevice->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoIoDevice->simulateExternalDisconnect();
    });
    timer.start(10);
    IoTransferDataGroup::Ptr workGroup = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup);
    QTest::qWait(50);

    QVERIFY(!queue.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(0, 0, 0, 1);
}

void IoQueueTest::disconnectWhileWorkingMultipleNotifications()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);
    demoIoDevice->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoIoDevice->simulateExternalDisconnect();
    });
    timer.start(10);
    IoTransferDataGroup::Ptr workGroup1 = generateSwitchCommands(true);
    IoTransferDataGroup::Ptr workGroup2 = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup1);
    queue.enqueueTransferGroup(workGroup2);
    QTest::qWait(50);

    QVERIFY(!queue.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(0, 0, 0, workGroup1->getTransferCount()+workGroup2->getTransferCount());
}

void IoQueueTest::stopOnFirstError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup::Ptr workTransferGroup =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList;
    transList.append(IoTransferDataSingleFactory::createIoData());
    transList.append(IoTransferDataSingleFactory::createIoData());
    transList.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup->appendTransferList(transList);
    workTransferGroup->setDemoErrorOnTransfer(1);

    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0]->passedAll(), false);
    QCOMPARE(m_listIoGroupsReceived[0]->getTransfer(0)->getEvaluation(), IoTransferDataSingle::EVAL_PASS);
    QCOMPARE(m_listIoGroupsReceived[0]->getTransfer(1)->getEvaluation(), IoTransferDataSingle::EVAL_WRONG_ANSWER);
    QCOMPARE(m_listIoGroupsReceived[0]->getTransfer(2)->getEvaluation(), IoTransferDataSingle::EVAL_NOT_EXECUTED);
}

void IoQueueTest::continueOnError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup::Ptr workTransferGroup =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_CONTINUE_ON_ERROR);
    tIoTransferList transList;
    transList.append(IoTransferDataSingleFactory::createIoData());
    transList.append(IoTransferDataSingleFactory::createIoData());
    transList.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup->appendTransferList(transList);
    workTransferGroup->setDemoErrorOnTransfer(1);

    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0]->passedAll(), false);
    QCOMPARE(m_listIoGroupsReceived[0]->getTransfer(0)->getEvaluation(), IoTransferDataSingle::EVAL_PASS);
    QCOMPARE(m_listIoGroupsReceived[0]->getTransfer(1)->getEvaluation(), IoTransferDataSingle::EVAL_WRONG_ANSWER);
    QCOMPARE(m_listIoGroupsReceived[0]->getTransfer(2)->getEvaluation(), IoTransferDataSingle::EVAL_PASS);
}

void IoQueueTest::noErrorSignalOnEmptyGroup()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup::Ptr workTransferGroup =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_CONTINUE_ON_ERROR);
    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(10);
    if(m_listIoGroupsReceived.count() != 1) {
        QCOMPARE(m_listIoGroupsReceived.count(), 1);
        QFAIL("Skipping check of error flag");
    }
    QVERIFY(m_listIoGroupsReceived[0]->passedAll());
}

void IoQueueTest::rejectSpam()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    constexpr int maxPendingGroups = 2;
    queue.setMaxPendingGroups(maxPendingGroups);

    IoTransferDataGroup::Ptr workTransferGroup1 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoTransferDataGroup::Ptr workTransferGroup2 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    IoTransferDataGroup::Ptr workTransferGroup3 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList3;
    transList3.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup3->appendTransferList(transList3);
    queue.enqueueTransferGroup(workTransferGroup3);

    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups+1);
    evalNotificationCount(maxPendingGroups, maxPendingGroups, 0, 1);
}

void IoQueueTest::acceptCloseToSpam()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    constexpr int maxPendingGroups = 2;
    queue.setMaxPendingGroups(maxPendingGroups);

    IoTransferDataGroup::Ptr workTransferGroup1 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoTransferDataGroup::Ptr workTransferGroup2 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups);
    evalNotificationCount(maxPendingGroups, maxPendingGroups, 0, 0);
}

void IoQueueTest::oneValidGroupSingleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup::Ptr workTransferGroup =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup);

    QVERIFY(queue.isIoBusy());
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, 1, 0, 0);
}

void IoQueueTest::twoValidGroupsSingleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup::Ptr workTransferGroup1 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoTransferDataGroup::Ptr workTransferGroup2 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QVERIFY(queue.isIoBusy());
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, 2, 0, 0);
}

void IoQueueTest::oneValidGroupMultipleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup::Ptr workTransferGroup =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup);

    QVERIFY(queue.isIoBusy());
    QTest::qWait(10);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, 2, 0, 0);
}

void IoQueueTest::twoValidGroupsMultipleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup::Ptr workTransferGroup1 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoTransferDataGroup::Ptr workTransferGroup2 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData());
    transList2.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QVERIFY(queue.isIoBusy());

    QTest::qWait(10);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, 4, 0, 0);
}

void IoQueueTest::twoFirstInvalidSecondOkSingleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    IoTransferDataGroup::Ptr workTransferGroup1 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->setDemoErrorOnTransfer(0);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoTransferDataGroup::Ptr workTransferGroup2 =
            IoTransferDataGroup::Ptr::create(IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingleFactory::createIoData());
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QTest::qWait(10);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &IoQueueTest::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(1, 1, 1, 0);
}

IoTransferDataGroup::Ptr IoQueueTest::generateStatusPollCommands()
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    return ioGroupGenerator.generateStatusPollGroup();
}

IoTransferDataGroup::Ptr IoQueueTest::generateSwitchCommands(bool on)
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
        if(m_listIoGroupsReceived[group]->passedAll()) {
            passedGroupCount++;
        }
        for(int io=0; io<m_listIoGroupsReceived[group]->getTransferCount(); ++io) {
            switch(m_listIoGroupsReceived[group]->getTransfer(io)->getEvaluation()) {
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

