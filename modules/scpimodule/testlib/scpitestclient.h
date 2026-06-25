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

    void sendScpiCmds(QString cmds);
    cSCPIInterface* getScpiInterface();
    int getUnhandledResponses() const;

    void handleCmdFinish(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) override;
signals:
    void sigScpiAnswer(const QString &scpiResponse);

private slots:
    void cmdInput() override;

private:
    int m_unhandledResponses = 0;
};

}

#endif // SCPITESTCLIENT_H
