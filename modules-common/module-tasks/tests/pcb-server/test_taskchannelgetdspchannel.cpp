#include "test_taskchannelgetdspchannel.h"
#include "taskchannelgetdspchannel.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskchannelgetdspchannel)

static const char* channelName = "m0";

void test_taskchannelgetdspchannel::checkScpiSend()
{
    PcbInitForTest pcb;
    std::shared_ptr<int> dspChannel = std::make_shared<int>();
    TaskTemplatePtr task = TaskChannelGetDspChannel::create(pcb.getPcbInterface(),
                                                            channelName,
                                                            dspChannel,
                                                            EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:DSPCHANNEL").arg(channelName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskchannelgetdspchannel::returnsDspChannelProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, QString("42")));
    std::shared_ptr<int> dspChannel = std::make_shared<int>();
    TaskTemplatePtr task = TaskChannelGetDspChannel::create(pcb.getPcbInterface(),
                                                            channelName,
                                                            dspChannel,
                                                            EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*dspChannel, 42);
}

void test_taskchannelgetdspchannel::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<int> dspChannel = std::make_shared<int>();
    TaskTemplatePtr task = TaskChannelGetDspChannel::create(pcb.getPcbInterface(),
                                                            channelName,
                                                            dspChannel,
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
