#ifndef SCPITESTCLIENT_H
#define SCPITESTCLIENT_H

#include "scpiclient.h"

namespace SCPIMODULE {

class ScpiTestClient : public cSCPIClient
{
    Q_OBJECT
public:
    ScpiTestClient(cSCPIModule* module, cSCPIModuleConfigData &configdata, cSCPIInterface* iface);

    void sendScpiCmds(QString cmds);
    cSCPIInterface* getScpiInterface();
    void handleCmdFinish(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) override;
signals:
    void sigScpiAnswer(const QString &scpiResponse);

private slots:
    void cmdInput() override;
};

}

#endif // SCPITESTCLIENT_H
