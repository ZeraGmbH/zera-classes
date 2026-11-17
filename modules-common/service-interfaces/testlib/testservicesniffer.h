#ifndef TESTSERVICESNIFFER_H
#define TESTSERVICESNIFFER_H

#include "testdspinterface.h"
#include <scpiconnection.h>
#include <vs_abstractdatabase.h>
#include <QJsonArray>

class TestServiceSniffer : public QObject
{
    Q_OBJECT
public:
    // DSP var write
    void sniffDspVarWrites(const QList<TestDspInterfacePtr> &testDspInterfaces, int limitVarSize=0);
    void sniffDspVarWrites(const TestDspInterfacePtr &testDspInterface, int limitVarSize=0);

    enum ScpiRecordype {
        queryOnly,
        cmdOnly,
        all
    };
    // Server SCPI
    void sniffScpiIoServer(ScpiConnection* server, ScpiRecordype type);

    // Vein
    void sniffVeinEntities(VeinStorage::AbstractDatabase* veinStorage, const QList<int> &entities);

    // Result
    QByteArray dump() const;

private:
    void onDspVariableWrite(const TestDspVarWriteData &dspVar, int limitVarSize);
    void onScpiCmdExecutionDone(const QString &scpi, const QString &response, ScpiRecordype type);
    void onVeinValueSet(int entityId, const QString &componentName, const QVariant &setValue);

    QJsonArray m_signalsCollected;
};

#endif // TESTSERVICESNIFFER_H
