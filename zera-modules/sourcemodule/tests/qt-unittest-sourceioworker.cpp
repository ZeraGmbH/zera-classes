#include "main-unittest-qt.h"
#include "qt-unittest-sourceioworker.h"

static QObject* pSourceIoWorkerTest = addTest(new SourceIoWorkerTest);

void SourceIoWorkerTest::init()
{

}

void SourceIoWorkerTest::testEmptyWorkerIsInvalid()
{
    cWorkerCommandPacket workPack;
    QVERIFY(workPack.m_commandType == COMMAND_UNDEFINED);
    QVERIFY(workPack.m_errorBehavior == BEHAVE_UNDEFINED);
}

void SourceIoWorkerTest::testCmdPackToWorkType()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceIoWorker::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_commandType, workPack.m_commandType);
}

void SourceIoWorkerTest::testCmdPackToWorkIoSize()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceIoWorker::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_singleOutInList.size(), workPack.m_workerIOList.size());
}

void SourceIoWorkerTest::testCmdPackToWorkIoSequence()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceIoWorker::commandPackToWorkerPack(cmdPack);
    for(int i=0; i<cmdPack.m_singleOutInList.size(); ++i) {
        QCOMPARE(cmdPack.m_singleOutInList[i].m_actionType, workPack.m_workerIOList[i].m_OutIn.m_actionType);
    }
}

void SourceIoWorkerTest::testCmdPackToWorkProperties()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceIoWorker::commandPackToWorkerPack(cmdPack);
    QVERIFY(workPack.m_commandType != COMMAND_UNDEFINED);
    QVERIFY(workPack.m_errorBehavior != BEHAVE_UNDEFINED);
}

void SourceIoWorkerTest::testNoInterfaceNotBusy()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceIoWorker::commandPackToWorkerPack(cmdPack);
    cSourceIoWorker worker;
    worker.enqueueIoPacket(workPack);
    QVERIFY(!worker.isBusy());
}

void SourceIoWorkerTest::testEmptyPackNotBusy()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    cWorkerCommandPacket workPack;
    worker.enqueueIoPacket(workPack);
    QVERIFY(!worker.isBusy());
}

static void enqueueSwitchCommands(cSourceIoWorker& worker, bool on) {
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(on);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerCommandPacket workPack = cSourceIoWorker::commandPackToWorkerPack(cmdPack);
    worker.enqueueIoPacket(workPack);
}

void SourceIoWorkerTest::testNotOpenInterfaceNotBusy()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    enqueueSwitchCommands(worker, false);
    QVERIFY(!worker.isBusy());
}

static tSourceInterfaceShPtr createOpenDevice() {
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    return interface;
}

void SourceIoWorkerTest::testOpenInterfaceBusy()
{
    cSourceIoWorker worker;
    worker.setIoInterface(createOpenDevice());
    enqueueSwitchCommands(worker, false);
    QVERIFY(worker.isBusy());
}

void SourceIoWorkerTest::testDisconnectBeforeEnqueue()
{
    tSourceInterfaceShPtr interface = createOpenDevice();
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    cSourceIoWorker worker;
    worker.setIoInterface(interface);
    demoInterface->simulateExternalDisconnect();
    enqueueSwitchCommands(worker, false);
    QVERIFY(!worker.isBusy());
}

