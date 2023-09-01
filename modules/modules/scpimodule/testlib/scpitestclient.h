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
public slots:
    void receiveAnswer(QString answ, bool ok = true) override;
private slots:
    void cmdInput() override;
};

}

#endif // SCPITESTCLIENT_H
