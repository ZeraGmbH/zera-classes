#include "test_taskrmchannelsgetavail.h"
#include "taskrmchannelsgetavail.h"
#include "rminterfacefortest.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_taskrmchannelsgetavail)

static const char* defaultResponse = "m0;m1;m2";

void test_taskrmchannelsgetavail::getThreeChannels()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

void test_taskrmchannelsgetavail::getThreeChannelsIgnoreMMode()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "m0;m1;m2;MMODE");
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QStringList channelList;
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

