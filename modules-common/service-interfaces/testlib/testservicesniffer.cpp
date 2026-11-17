#include "testservicesniffer.h"
#include <testloghelpers.h>
#include <timemachinefortest.h>
#include <vs_dumpjson.h>
#include <QString>

void TestServiceSniffer::sniffDspVarWrites(const QList<TestDspInterfacePtr> &testDspInterfaces, int limitVarSize)
{
    for (const TestDspInterfacePtr &interface : testDspInterfaces)
        connect(interface.get(), &TestDspInterface::sigDspVarWrite, this, [=](const TestDspVarWriteData &dspVar) {
            onDspVariableWrite(dspVar, limitVarSize);
        });
}

void TestServiceSniffer::sniffDspVarWrites(const TestDspInterfacePtr &testDspInterface, int limitVarSize)
{
    connect(testDspInterface.get(), &TestDspInterface::sigDspVarWrite, this, [=](const TestDspVarWriteData &dspVar) {
        onDspVariableWrite(dspVar, limitVarSize);
    });
}

void TestServiceSniffer::sniffScpiIoServer(ScpiConnection *server, ScpiRecordype type)
{
    connect(server, &ScpiConnection::sigNotifyScpiIo, this, [=](const QString &scpi, const QString &response) {
        onScpiCmdExecutionDone(scpi, response, type);
    });
}

void TestServiceSniffer::sniffVeinEntities(VeinStorage::AbstractDatabase *veinStorage, const QList<int> &entities)
{
    for (int entityId : entities) {
        const QStringList componentNames = veinStorage->getComponentList(entityId);
        for (const QString &componentName : componentNames) {
            const VeinStorage::AbstractComponentPtr veinComponent = veinStorage->findComponent(entityId, componentName);
            connect(veinComponent.get(), &VeinStorage::AbstractComponent::sigValueSet, this, [=](const QVariant &setValue) {
                onVeinValueSet(entityId, componentName, setValue);
            });
        }
    }
}

QByteArray TestServiceSniffer::dump() const
{
    QJsonObject json;
    json["signals"] = m_signalsCollected;
    return TestLogHelpers::dump(json);
}

void TestServiceSniffer::onDspVariableWrite(const TestDspVarWriteData &dspVar, int limitVarSize)
{
    QJsonObject jsonDspVar;
    jsonDspVar["is"] = "DspVarWrite";
    jsonDspVar["varName"] = dspVar.varName;
    jsonDspVar["timeMs"] = TimeMachineForTest::getInstance()->getCurrentTimeMs();
    jsonDspVar["transActionNo"] = dspVar.transactionCount;

    QJsonArray values;
    int arrMax = dspVar.values.count();
    if (limitVarSize != 0 && limitVarSize<arrMax)
        arrMax = limitVarSize;

    for (int i=0; i<arrMax; ++i)
        values.append(QString::number(dspVar.values[i], 'g', 7).toDouble()); // We cannot add more digits - why exactly???
    jsonDspVar["varValues"] = values;

    m_signalsCollected.append(jsonDspVar);
}

void TestServiceSniffer::onScpiCmdExecutionDone(const QString &scpi, const QString &response, ScpiRecordype type)
{
    QJsonObject jsonScpiVar;
    cSCPICommand cmd = scpi;
    bool isQuery = cmd.isQuery(0) || cmd.isQuery(1);
    bool doLog = false;
    if (isQuery && (type == queryOnly  || type == all))
        doLog = true;
    else if (!isQuery && (type == cmdOnly || type == all))
        doLog = true;

    if (doLog) {
        jsonScpiVar["is"] = "ScpiServiceIo";
        jsonScpiVar["scpiCmd"] = scpi;
        jsonScpiVar["scpiResponse"] = response;
        jsonScpiVar["timeMs"] = TimeMachineForTest::getInstance()->getCurrentTimeMs();
        m_signalsCollected.append(jsonScpiVar);
    }
}

void TestServiceSniffer::onVeinValueSet(int entityId, const QString &componentName, const QVariant &setValue)
{
    QJsonObject jsonVeinSet;
    jsonVeinSet["is"] = "VeinSet";
    jsonVeinSet["veinEntityId"] = entityId;
    jsonVeinSet["veinComponentName"] = componentName;
    jsonVeinSet["veinValue"] = VeinStorage::DumpJson::convertToJsonValue(setValue);
    jsonVeinSet["timeMs"] = TimeMachineForTest::getInstance()->getCurrentTimeMs();
    m_signalsCollected.append(jsonVeinSet);
}
