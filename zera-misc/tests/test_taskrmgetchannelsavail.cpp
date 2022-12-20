#include "test_taskrmgetchannelsavail.h"
#include "taskrmchannelsgetavail.h"
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
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(rmInterface,
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
    TaskCompositePtr task = TaskRmChannelsGetAvail::create(rmInterface,
                                                           DELAY_TIME,
                                                           channelList);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(channelList.count(), 3);
    QStringList expectedChannels = QString(defaultResponse).split(";");
    QCOMPARE(channelList, expectedChannels);
}

