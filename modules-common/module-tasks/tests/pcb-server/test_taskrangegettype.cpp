#include "test_taskrangegettype.h"
#include "taskrangegettype.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskrangegettype)

static const char* channelMName = "m0";
static const char* rangeName = "250V";
static const int defaultType = 42;

void test_taskrangegettype::checkScpiSend()
{
    PcbInitForTest pcb;
    std::shared_ptr<int> type = std::make_shared<int>();
    TaskTemplatePtr task = TaskRangeGetType::create(pcb.getPcbInterface(),
                                                           channelMName, rangeName,
                                                           type,
                                                           EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SENSE:%1:%2:TYPE").arg(channelMName, rangeName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskrangegettype::returnsTypeProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, QString("%1").arg(defaultType)));
    std::shared_ptr<int> type = std::make_shared<int>(0);
    TaskTemplatePtr task = TaskRangeGetType::create(pcb.getPcbInterface(),
                                                    channelMName, rangeName,
                                                    type,
                                                    EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(*type, defaultType);
}

void test_taskrangegettype::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<int> type = std::make_shared<int>(0);
    TaskTemplatePtr task = TaskRangeGetType::create(pcb.getPcbInterface(),
                                                    channelMName, rangeName,
                                                    type,
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
