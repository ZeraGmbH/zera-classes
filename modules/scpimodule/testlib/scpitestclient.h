#ifndef SCPITESTCLIENT_H
#define SCPITESTCLIENT_H

#include "scpiclient.h"

namespace SCPIMODULE {

// Enhanced SCPI I/O designed to anlayse insights. If tests require just I/O, see ScpiModuleClientBlocked
class ScpiTestClient : public cSCPIClient
{
    Q_OBJECT
public:
    ScpiTestClient(cSCPIModule* module, cSCPIModuleConfigData &configdata, cSCPIInterface* iface);

    void handleCmdFinish(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) override;
    cSCPIInterface* getScpiInterface();

    void sendScpiCmds(QString cmds);
    const QString &getLastResponse() const;
    int getUnhandledResponses() const;
signals:
    void sigScpiAnswer(const QString &scpiResponse);

private slots:
    void cmdInput() override;

private:
    QString m_lastResponse;
    int m_unhandledResponses = 0;
};

}

#endif // SCPITESTCLIENT_H
