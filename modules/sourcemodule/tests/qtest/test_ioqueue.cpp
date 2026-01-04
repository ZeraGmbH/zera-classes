#include "test_ioqueue.h"
#include "test_globals.h"
#include "iodevicefactory.h"
#include "iodevicedemo.h"
#include "iogroupgenerator.h"
#include "timerfactoryqt.h"
#include "timemachinefortest.h"
#include "timerfactoryqtfortest.h"

QTEST_MAIN(test_ioqueue)

void test_ioqueue::onIoQueueGroupFinished(IoQueueGroup::Ptr workGroup)
{
    m_listIoGroupsReceived.append(workGroup);
}

void test_ioqueue::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_ioqueue::init()
{
    m_listIoGroupsReceived.clear();
    TimerFactoryQtForTest::enableTest();
}

void test_ioqueue::noIoDeviceNotBusy()
{
    IoQueueGroup::Ptr workGroup = generateSwitchCommands(true);
    IoQueue queue;
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void test_ioqueue::emptyGroupNotBusy()
{
    IoQueue queue;
    queue.setIoDevice(createOpenDemoIoDevice());
    IoQueueGroup::Ptr workGroup = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void test_ioqueue::nullGroupNotBusy()
{
    IoQueue queue;
    queue.setIoDevice(createOpenDemoIoDevice());
    IoQueueGroup::Ptr null;
    queue.enqueueTransferGroup(null);
    QVERIFY(!queue.isIoBusy());
}

void test_ioqueue::notOpenIoDeviceNotBusy()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    IoQueueGroup::Ptr workGroup = generateSwitchCommands(false);
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(!queue.isIoBusy());
}

void test_ioqueue::openIoDeviceBusy()
{
    IoQueue queue;
    IoQueueGroup::Ptr workGroup = generateSwitchCommands(false);
    queue.setIoDevice(createOpenDemoIoDevice());
    queue.enqueueTransferGroup(workGroup);
    QVERIFY(queue.isIoBusy());
}

void test_ioqueue::noIoDeviceNotification()
{
    IoQueue queue;
    IoQueueGroup::Ptr workGroup = generateSwitchCommands(true);

    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);
    queue.enqueueTransferGroup(workGroup);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    QVERIFY(m_listIoGroupsReceived[0].get() == workGroup.get());
    evalNotificationCount(0, 0, 0, workGroup->getTransferCount());
}

void test_ioqueue::notOpenIoDeviceNotifications()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    IoQueue queue;
    IoQueueGroup::Ptr workGroup = generateSwitchCommands(true);
    queue.setIoDevice(ioDevice);

    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);
    queue.enqueueTransferGroup(workGroup);

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
    IoQueueGroup::Ptr workGroup = generateSwitchCommands(false);
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

    TimerTemplateQtPtr timer = TimerFactoryQt::createSingleShot(10);
    connect(timer.get(), &TimerTemplateQt::sigExpired, timer.get(), [&]() {
        demoIoDevice->close();
    });
    timer->start();
    IoQueueGroup::Ptr workGroup = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup);
    TimeMachineForTest::getInstance()->processTimers(50);

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

    TimerTemplateQtPtr timer = TimerFactoryQt::createSingleShot(10);
    connect(timer.get(), &TimerTemplateQt::sigExpired, timer.get(), [&]() {
        demoIoDevice->close();
    });
    timer->start();
    IoQueueGroup::Ptr workGroup1 = generateSwitchCommands(true);
    IoQueueGroup::Ptr workGroup2 = generateSwitchCommands(false);
    queue.enqueueTransferGroup(workGroup1);
    queue.enqueueTransferGroup(workGroup2);
    TimeMachineForTest::getInstance()->processTimers(50);

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

    IoQueueGroup::Ptr workTransferGroup =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList;
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList);
    workTransferGroup->getTransfer(1)->getDemoResponder()->activateErrorResponse();

    queue.enqueueTransferGroup(workTransferGroup);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

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

    IoQueueGroup::Ptr workTransferGroup =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_FIRST_OK);
    tIoTransferList transList;
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList);
    workTransferGroup->getTransfer(0)->getDemoResponder()->activateErrorResponse();

    queue.enqueueTransferGroup(workTransferGroup);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

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

    IoQueueGroup::Ptr workTransferGroup =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::CONTINUE_ON_ERROR);
    tIoTransferList transList;
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    transList.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList);
    workTransferGroup->getTransfer(1)->getDemoResponder()->activateErrorResponse();

    queue.enqueueTransferGroup(workTransferGroup);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

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

    IoQueueGroup::Ptr workTransferGroup =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::CONTINUE_ON_ERROR);
    queue.enqueueTransferGroup(workTransferGroup);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
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

    IoQueueGroup::Ptr workTransferGroup1 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueGroup::Ptr workTransferGroup2 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    IoQueueGroup::Ptr workTransferGroup3 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList3;
    transList3.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup3->appendTransferList(transList3);
    queue.enqueueTransferGroup(workTransferGroup3);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

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

    IoQueueGroup::Ptr workTransferGroup1 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueGroup::Ptr workTransferGroup2 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), maxPendingGroups);
    evalNotificationCount(maxPendingGroups, maxPendingGroups, 0, 0);
}

void test_ioqueue::oneValidGroupSingleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueGroup::Ptr workTransferGroup =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup);

    QVERIFY(queue.isIoBusy());
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, 1, 0, 0);
}

void test_ioqueue::twoValidGroupsSingleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueGroup::Ptr workTransferGroup1 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueGroup::Ptr workTransferGroup2 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QVERIFY(queue.isIoBusy());
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(2, 2, 0, 0);
}

void test_ioqueue::oneValidGroupMultipleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueGroup::Ptr workTransferGroup =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup);

    QVERIFY(queue.isIoBusy());
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);

    QCOMPARE(m_listIoGroupsReceived.count(), 1);
    evalNotificationCount(1, 2, 0, 0);
}

void test_ioqueue::twoValidGroupsMultipleIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    IoQueue queue;
    queue.setIoDevice(ioDevice);
    connect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    IoQueueGroup::Ptr workTransferGroup1 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueGroup::Ptr workTransferGroup2 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    QVERIFY(queue.isIoBusy());

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
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

    IoQueueGroup::Ptr workTransferGroup1 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList1;
    transList1.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup1->appendTransferList(transList1);
    workTransferGroup1->getTransfer(0)->getDemoResponder()->activateErrorResponse();
    queue.enqueueTransferGroup(workTransferGroup1);

    IoQueueGroup::Ptr workTransferGroup2 =
            IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_ERROR);
    tIoTransferList transList2;
    transList2.append(IoTransferDataSingle::Ptr::create("", ""));
    workTransferGroup2->appendTransferList(transList2);
    queue.enqueueTransferGroup(workTransferGroup2);

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    disconnect(&queue, &IoQueue::sigTransferGroupFinished, this, &test_ioqueue::onIoQueueGroupFinished);

    QCOMPARE(m_listIoGroupsReceived.count(), 2);
    evalNotificationCount(1, 1, 1, 0);
}

IoQueueGroup::Ptr test_ioqueue::generateStatusPollCommands()
{
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    return ioGroupGenerator.generateStatusPollGroup();
}

IoQueueGroup::Ptr test_ioqueue::generateSwitchCommands(bool on)
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

