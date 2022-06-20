#include "test_ioqueue.h"
#include "test_globals.h"
#include "io-device/iodevicefactory.h"
#include "io-device/iodevicedemo.h"
#include "source-protocols/iogroupgenerator.h"

QTEST_MAIN(test_ioqueue)

void test_ioqueue::onIoQueueGroupFinished(IoQueueEntry::Ptr workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void test_ioqueue::init()
{
    m_listIoGroupsReceived.clear();
}

void test_ioqueue::noIoDeviceNotBusy()
{
    IoQueueEntry::Ptr workGroup = generateSwitchCommands(true);
    IoQueue queue;
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void test_ioqueue::emptyGroupNotBusy()
{
    IoQueue queue;
    queue.setIoDevice(createOpenDemoIoDevice());
    IoQueueEntry::Ptr workGroup = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void test_ioqueue::nullGroupNotBusy()
{
    IoQueue queue;
    queue.setIoDevice(createOpenDemoIoDevice());
    IoQueueEntry::Ptr null;
    queue.enqueueTransferGroup(null);
    QVERIFY(!queue.isIoBusy());
}

void test_ioqueue::notOpenIoDeviceNotBusy()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    IoQueueEntry::Ptr workGroup = generateSwitchCommands(false);
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void test_ioqueue::openIoDeviceBusy()
{
    IoQueue queue;
    IoQueueEntry::Ptr workGroup = generateSwitchCommands(false);
    queue.setIoDevice(createOpenDemoIoDevice());
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(queue.isIoBusy());
}

void test_ioqueue::noIoDeviceNotification()
{
    IoQueue queue;
    IoQueueEntry::Ptr workGroup = generateSwitchCommands(true);

    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);
    queue.enqueueTransferGroup(workGroup);
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0].get() == workGroup.get());
    evalNotificationCount(0, 0, 0, workGroup->getTransferCount());
}

void test_ioqueue::notOpenIoDeviceNotifications()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    IoQueue queue;
    IoQueueEntry::Ptr workGroup = generateSwitchCommands(true);
    queue.setIoDevice(ioDevice);

    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);
    queue.enqueueTransferGroup(workGroup);
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0] == workGroup);
    evalNotificationCount(0, 0, 0, workGroup->getTransferCount());
}

void test_ioqueue::disconnectBeforeEnqueue()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    IoQueue queue;
    queue.setIoDevice(ioDevice);

    demoIoDevice->close();
    IoQueueEntry::Ptr workGroup = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup);

    QVERIFY(!queue.isIoBusy());
}

void test_ioqueue::disconnectWhileWorking()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);
    demoIoDevice->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoIoDevice->close();
    });
    timer.start(10);
    IoQueueEntry::Ptr workGroup = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup);
    QTest::qWait(50);

    QVERIFY(!queue.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(0, 0, 0, 1);
}

void test_ioqueue::disconnectWhileWorkingMultipleNotifications()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);
    demoIoDevice->setResponseDelay(false, 500);

    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, [&]() {
        demoIoDevice->close();
    });
    timer.start(10);
    IoQueueEntry::Ptr workGroup1 = generateSwitchCommands(true);
    IoQueueEntry::Ptr workGroup2 = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup1);
    queue.enqueueTransferGroup(workGroup2);
    QTest::qWait(50);

    QVERIFY(!queue.isIoBusy());
    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(0, 0, 0, workGroup1->getTransferCount()+workGroup2->getTransferCount());
}

void test_ioqueue::stopOnFirstError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueEntry::Ptr workTransferGroup =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList;
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList);
    workTransferGroup->getTransfer(1)->getDemoResponder()->activateErrorResponse();

    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0]->passedAll(), false);
    QVERIFY(m_listIoGroupsReceived[0]->getTransfer(0)->didIoPass());
    QVERIFY(m_listIoGroupsReceived[0]->getTransfer(1)->wrongAnswerReceived());
    QVERIFY(m_listIoGroupsReceived[0]->getTransfer(2)->wasNotRunYet());
}

void test_ioqueue::stopOnFirstOk()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueEntry::Ptr workTransferGroup =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_FIRST_OK);
    tIoTransferList transList;
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList);
    workTransferGroup->getTransfer(0)->getDemoResponder()->activateErrorResponse();

    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0]->passedAll(), false);
    QVERIFY(m_listIoGroupsReceived[0]->getTransfer(0)->wrongAnswerReceived());
    QVERIFY(m_listIoGroupsReceived[0]->getTransfer(1)->didIoPass());
    QVERIFY(m_listIoGroupsReceived[0]->getTransfer(2)->wasNotRunYet());
}

void test_ioqueue::continueOnError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueEntry::Ptr workTransferGroup =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::CONTINUE_ON_ERROR);
    tIoTransferList transList;
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList);
    workTransferGroup->getTransfer(1)->getDemoResponder()->activateErrorResponse();

    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QCOMPARE(m_listIoGroupsReceived[0]->passedAll(), false);
    QVERIFY(m_listIoGroupsReceived[0]->getTransfer(0)->didIoPass());
    QVERIFY(m_listIoGroupsReceived[0]->getTransfer(1)->wrongAnswerReceived());
    QVERIFY(m_listIoGroupsReceived[0]->getTransfer(2)->didIoPass());
}

void test_ioqueue::noErrorSignalOnEmptyGroup()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueEntry::Ptr workTransferGroup =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::CONTINUE_ON_ERROR);
    queue.enqueueTransferGroup(workTransferGroup);
    QTest::qWait(shortQtEventTimeout);
    if(m_listIoGroupsReceived.count() != 1) {
        QCOMPARE(m_listIoGroupsReceived.count(), 1);
        QFAIL("Skipping check of error flag");
    }
    QVERIFY(m_listIoGroupsReceived[0]->passedAll());
}

void test_ioqueue::rejectSpam()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    constexpr int maxPendingGroups = 2;
    queue.setMaxPendingGroups(maxPendingGroups);

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    IoQueueEntry::Ptr workTransferGroup3 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList3;
    transList3.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup3->appendTransferList(transList3);
    queue.enqueueTransferGroup(workTransferGroup3);

    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups+1);
    evalNotificationCount(maxPendingGroups, maxPendingGroups, 0, 1);
}

void test_ioqueue::acceptCloseToSpam()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    constexpr int maxPendingGroups = 2;
    queue.setMaxPendingGroups(maxPendingGroups);

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups);
    evalNotificationCount(maxPendingGroups, maxPendingGroups, 0, 0);
}

void test_ioqueue::oneValidGroupSingleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueEntry::Ptr workTransferGroup =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup);

    QVERIFY(queue.isIoBusy());
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, 1, 0, 0);
}

void test_ioqueue::twoValidGroupsSingleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QVERIFY(queue.isIoBusy());
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, 2, 0, 0);
}

void test_ioqueue::oneValidGroupMultipleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueEntry::Ptr workTransferGroup =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup);

    QVERIFY(queue.isIoBusy());
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, 2, 0, 0);
}

void test_ioqueue::twoValidGroupsMultipleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QVERIFY(queue.isIoBusy());

    QTest::qWait(shortQtEventTimeout);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, 4, 0, 0);
}

void test_ioqueue::twoFirstInvalidSecondOkSingleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueEntry::Ptr workTransferGroup1 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueEntry::Ptr workTransferGroup2 =
            IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QTest::qWait(shortQtEventTimeout);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(1, 1, 1, 0);
}

IoQueueEntry::Ptr test_ioqueue::generateStatusPollCommands()
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    return ioGroupGenerator.generateStatusPollGroup();
}

IoQueueEntry::Ptr test_ioqueue::generateSwitchCommands(bool on)
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(on);
    return ioGroupGenerator.generateOnOffGroup(params);
}

void test_ioqueue::evalNotificationCount(int passedGroupsExpected, int passExpected, int failExpected, int unknownExpected)
{
    int passedGroupCount = 0, unknownCount = 0, passCount = 0, failCount = 0;
    for(int group=0; group<m_listIoGroupsReceived.count(); group++) {
        if(m_listIoGroupsReceived[group]->passedAll()) {
            passedGroupCount++;
        }
        for(int io=0; io<m_listIoGroupsReceived[group]->getTransferCount(); ++io) {
            if(m_listIoGroupsReceived[group]->getTransfer(io)->wasNotRunYet()) {
                unknownCount++;
            }
            else if(m_listIoGroupsReceived[group]->getTransfer(io)->didIoPass()) {
                passCount++;
            }
            else if(m_listIoGroupsReceived[group]->getTransfer(io)->wrongAnswerReceived()) {
                failCount++;
            }
        }
    }
    QCOMPARE(passedGroupCount, passedGroupsExpected);
    QCOMPARE(passCount, passExpected);
    QCOMPARE(failCount, failExpected);
    QCOMPARE(unknownCount, unknownExpected);
}

