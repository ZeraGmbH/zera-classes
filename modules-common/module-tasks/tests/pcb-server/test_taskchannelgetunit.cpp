#include "test_taskchannelgetunit.h"
#include "taskchannelgetunit.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskchannelgetunit)

static const char* channelName = "m0";
static const char* defaultResponse = "V";

void test_taskchannelgetunit::checkScpiSend()
{
    PcbInitForTest pcb;
    std::shared_ptr<QString> channelAlias = std::make_shared<QString>();
    TaskTemplatePtr task = TaskChannelGetUnit::create(pcb.getPcbInterface(),
                                                      channelName,
                                                      channelAlias,
                                                      EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:UNIT").arg(channelName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskchannelgetunit::returnsAliasProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, QString(defaultResponse)));
    std::shared_ptr<QString> channelAlias = std::make_shared<QString>();
    TaskTemplatePtr task = TaskChannelGetUnit::create(pcb.getPcbInterface(),
                                                      channelName,
                                                      channelAlias,
                                                      EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*channelAlias, defaultResponse);
}

void test_taskchannelgetunit::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<QString> channelAlias = std::make_shared<QString>();
    TaskTemplatePtr task = TaskChannelGetUnit::create(pcb.getPcbInterface(),
                                                      channelName,
                                                      channelAlias,
                                                      DEFAULT_EXPIRE,
                                                      [&]{
                                                           localErrorCount++;
                                                      });
    TaskTestHelper helper(task.get());
    task->start();
    TimeMachineForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}
