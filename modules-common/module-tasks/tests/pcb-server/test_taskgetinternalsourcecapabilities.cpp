#include "test_taskgetinternalsourcecapabilities.h"
#include "taskgetinternalsourcecapabilities.h"
#include <pcbinitfortest.h>
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskgetinternalsourcecapabilities)

void test_taskgetinternalsourcecapabilities::checkScpiSend()
{
    PcbInitForTest pcb;
    std::shared_ptr<QJsonObject> jsonCapabilites = std::make_shared<QJsonObject>();
    TaskTemplatePtr task = TaskGetInternalSourceCapabilities::create(pcb.getPcbInterface(),
                                                                     jsonCapabilites,
                                                                     EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("UISRC:CAPABILITIES");
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskgetinternalsourcecapabilities::returnsCapabilitiesProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, "{\"foo\": 1}"));
    std::shared_ptr<QJsonObject> jsonCapabilites = std::make_shared<QJsonObject>();
    TaskTemplatePtr task = TaskGetInternalSourceCapabilities::create(pcb.getPcbInterface(),
                                                                     jsonCapabilites,
                                                                     EXPIRE_INFINITE);
    task->start();
    TimeMachineObject::feedEventLoop();
    QVERIFY(!jsonCapabilites->isEmpty());
}

void test_taskgetinternalsourcecapabilities::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<QJsonObject> jsonCapabilites = std::make_shared<QJsonObject>();
    TaskTemplatePtr task = TaskGetInternalSourceCapabilities::create(pcb.getPcbInterface(),
                                                                     jsonCapabilites,
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
