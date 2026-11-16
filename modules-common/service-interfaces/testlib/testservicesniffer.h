#ifndef TESTSERVICESNIFFER_H
#define TESTSERVICESNIFFER_H

#include "testdspinterface.h"
#include <scpiconnection.h>
#include <QJsonArray>

class TestServiceSniffer : public QObject
{
    Q_OBJECT
public:
    void sniffDspVarWrites(const QList<TestDspInterfacePtr> &testDspInterfaces, int limitVarSize=0);
    void sniffDspVarWrites(const TestDspInterfacePtr &testDspInterface, int limitVarSize=0);

    enum ScpiRecordype {
        queryOnly,
        cmdOnly,
        all
    };
    void sniffScpiIoServer(ScpiConnection* server, ScpiRecordype type);

    QByteArray dump() const;

private:
    void onDspVariableWrite(const TestDspVarWriteData &dspVar, int limitVarSize);
    void onScpiCmdExecutionDone(const QString &scpi, const QString &response, ScpiRecordype type);

    QJsonArray m_signalsCollected;
};

#endif // TESTSERVICESNIFFER_H
