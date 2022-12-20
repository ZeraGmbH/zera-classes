#include "test_taskrmgetchannelsavail.h"
#include "taskrmgetchannelsavail.h"
#include "rminterfacefortest.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_taskrmgetchannelsavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmgetchannelsavail::getThreeChannels()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmGetChannelsAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(channelList.count(), 3);
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

void test_taskrmgetchannelsavail::getThreeChannelsIgnoreMMode()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "m0;m1;m2;MMODE");
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmGetChannelsAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(channelList.count(), 3);
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

void test_taskrmgetchannelsavail::noChannelsOnNack()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(nack, defaultResponse);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmGetChannelsAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(channelList.count(), 0);
}

void test_taskrmgetchannelsavail::okReceiveOnAck()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmGetChannelsAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmgetchannelsavail::errReceiveOnNack()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(nack, defaultResponse);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmGetChannelsAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskrmgetchannelsavail::noReceiveOnOther()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse, RmTestAnswer::MSG_ID_OTHER);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmGetChannelsAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmgetchannelsavail::errReceiveOnTcpError()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse, RmTestAnswer::TCP_ERROR);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmGetChannelsAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

