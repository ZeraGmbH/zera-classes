#include "test_taskchannelgetalias.h"
#include "taskchannelgetalias.h"
#include "pcbinitfortest.h"
#include <QTest>

QTEST_MAIN(test_taskchannelgetalias)

static const char* channelName = "m0";
static const char* defaultResponse = "UL1";

void test_taskchannelgetalias::checkScpiSend()
{
    PcbInitForTest pcb;
    QString channelAlias;
    TaskCompositePtr task = TaskChannelGetAlias::create(pcb.getPcbInterface(),
                                                        channelName,
                                                        channelAlias,
                                                        EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:ALIAS").arg(channelName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskchannelgetalias::returnsAliasProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, QString(defaultResponse)));
    QString channelAlias;
    TaskCompositePtr task = TaskChannelGetAlias::create(pcb.getPcbInterface(),
                                                        channelName,
                                                        channelAlias,
                                                        EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(channelAlias, defaultResponse);
}

void test_taskchannelgetalias::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    QString channelAlias;
    TaskCompositePtr task = TaskChannelGetAlias::create(pcb.getPcbInterface(),
                                                        channelName,
                                                        channelAlias,
                                                        DEFAULT_EXPIRE,
                                                        [&]{
        localErrorCount++;
    });
    TaskTestHelper helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}
