#include "test_taskrefpowerfetchconstant.h"
#include "taskrefpowerfetchconstant.h"
#include "pcbinitfortest.h"
#include "scpifullcmdcheckerfortest.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskrefpowerfetchconstant)

void test_taskrefpowerfetchconstant::checkScpiSend()
{
    PcbInitForTest pcb;
    std::shared_ptr<double> refContantReceived = std::make_shared<double>();
    QString refPowerName = QStringLiteral("InputFoo");
    TaskTemplatePtr task = TaskRefPowerFetchConstant::create(pcb.getPcbInterface(),
                                                             refPowerName,
                                                             refContantReceived,
                                                             EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QStringList scpiSent = pcb.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    QString scpiExpectedPath = QString("SOUR:%1:CONS").arg(refPowerName);
    ScpiFullCmdCheckerForTest scpiChecker(scpiExpectedPath, SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

static const char* defaultResponse = "42.1";

void test_taskrefpowerfetchconstant::returnsConstantProperly()
{
    PcbInitForTest pcb;
    pcb.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, QString(defaultResponse)));
    std::shared_ptr<double> refContantReceived = std::make_shared<double>();
    QString refPowerName = QStringLiteral("InputFoo");
    TaskTemplatePtr task = TaskRefPowerFetchConstant::create(pcb.getPcbInterface(),
                                                             refPowerName,
                                                             refContantReceived,
                                                             EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    double dblConstantExpected = QString(defaultResponse).toDouble();
    QCOMPARE(*refContantReceived, dblConstantExpected);
}

void test_taskrefpowerfetchconstant::timeoutAndErrFunc()
{
    PcbInitForTest pcb;
    int localErrorCount = 0;
    std::shared_ptr<double> refContantReceived = std::make_shared<double>();
    QString refPowerName = QStringLiteral("InputFoo");
    TaskTemplatePtr task = TaskRefPowerFetchConstant::create(pcb.getPcbInterface(),
                                                             refPowerName,
                                                             refContantReceived,
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
