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
signals:
    void sigScpiAnswer(QString answ);

private slots:
    void cmdInput() override;
    void handleCmdFinish(QString answ, const ScpiTransactionId &scpiTransactionId, bool ok = true, bool skipLog = false) override;
};

}

#endif // SCPITESTCLIENT_H
