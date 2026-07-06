#ifndef SCPITESTCLIENT_H
#define SCPITESTCLIENT_H

#include "scpiclient.h"

namespace SCPIMODULE {

// Enhanced SCPI I/O designed to anlayse insights. If tests require just I/O, see ScpiModuleNetClientBlocked
class ScpiTestClient : public cSCPIClient
{
    Q_OBJECT
public:
    ScpiTestClient(cSCPIModule* module, cSCPIModuleConfigData &configdata, cSCPIInterface* iface);

    void handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) override;
    cSCPIInterface* getScpiInterface();

    void sendScpiCmds(QString cmds);
    const NullableString &getLastResponse() const;
    int getHandledResponses() const;
    int getUnhandledResponses() const;
    bool getAtLeastOneResponse() const;
signals:
    void sigScpiAnswer(const QString &scpiResponse, bool isNull);

private slots:
    void cmdInput() override;

private:
    NullableString m_lastResponse;
    int m_handledResponses = 0;
    int m_unhandledResponses = 0;
};

}

#endif // SCPITESTCLIENT_H
