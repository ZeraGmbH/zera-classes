#include "scpitestclient.h"

namespace SCPIMODULE {

ScpiTestClient::ScpiTestClient(cSCPIModule *module, cSCPIModuleConfigData &configdata, cSCPIInterface *iface) :
    cSCPIClient(module, configdata, iface)
{
}

void ScpiTestClient::sendScpiCmds(QString cmds)
{
    QTextStream stream(&cmds);
    QString line;
    while (stream.readLineInto(&line)) {
        if(!line.endsWith("\n"))
            line += "\n";
        m_sInputFifo.append(line);
    }
    testCmd();
}

cSCPIInterface* ScpiTestClient::getScpiInterface()
{
    return m_pSCPIInterface;
}

void ScpiTestClient::receiveAnswer(QString answ, bool ok)
{
    emit sigScpiAnswer(answ);
    if(ok)
        emit commandAnswered(this);
}

void ScpiTestClient::cmdInput()
{
    // unused - does not match use case....
}

}
