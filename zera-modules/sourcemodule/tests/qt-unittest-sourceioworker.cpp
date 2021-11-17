#include "main-unittest-qt.h"
#include "qt-unittest-sourceioworker.h"

static QObject* pSourceIoWorkerTest = addTest(new SourceIoWorkerTest);

void SourceIoWorkerTest::init()
{

}

void SourceIoWorkerTest::testCmdPackToWorkType()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerPacket workPack = cSourceIoWorker::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_commandType, workPack.m_commandType);
}

void SourceIoWorkerTest::testCmdPackToWorkIoSize()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerPacket workPack = cSourceIoWorker::commandPackToWorkerPack(cmdPack);
    QCOMPARE(cmdPack.m_singleOutInList.size(), workPack.m_workerList.size());
}

void SourceIoWorkerTest::testCmdPackToWorkIoSequence()
{
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(true);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    cWorkerPacket workPack = cSourceIoWorker::commandPackToWorkerPack(cmdPack);
    for(int i=0; i<cmdPack.m_singleOutInList.size(); ++i) {
        QCOMPARE(cmdPack.m_singleOutInList[i].m_actionType, workPack.m_workerList[i].m_OutIn.m_actionType);
    }
}

