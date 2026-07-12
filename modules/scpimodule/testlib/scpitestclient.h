#ifndef SCPITESTCLIENT_H
#define SCPITESTCLIENT_H

#include "scpiclient.h"

namespace SCPIMODULE {

// Enhanced SCPI I/O designed to anlayse insights. If tests require just I/O, see ScpiModuleNetClientBlocked
class ScpiTestClient : public cSCPIClient
{
    Q_OBJECT
public:
    ScpiTestClient(cSCPIModule* module);
    ~ScpiTestClient() override;

    void handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) override;
    cSCPIInterface* getScpiInterface();

    void sendScpiCmds(QString cmds);

    const NullableStringList &getResponsesNotSorted() const;
    const NullableStringList &getResponsesSorted() const;

    int getHandledResponses() const;
    int getUnhandledResponses() const;
    bool getAtLeastOneResponse() const;
signals:
    void sigScpiResponseNotSorted(const QString &scpiResponse, bool isNull);
    void sigScpiResponseSorted(const QString &scpiResponse, bool isNull);

private slots:
    void cmdInput() override;

private:
    NullableStringList m_responseNotSorted;
    NullableStringList m_responsesSorted;
    int m_handledResponses = 0;
    int m_unhandledResponses = 0;
};

}

#endif // SCPITESTCLIENT_H
