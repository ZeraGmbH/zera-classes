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
    execPendingCmds();
}

cSCPIInterface* ScpiTestClient::getScpiInterface()
{
    return m_pSCPIInterface;
}

void ScpiTestClient::handleCmdFinish(QString answ, const ScpiTransactionId &scpiTransactionId, bool ok, bool skipLog)
{
    Q_UNUSED(scpiTransactionId)
    Q_UNUSED(skipLog)
    emit sigScpiAnswer(answ);
}

void ScpiTestClient::cmdInput()
{
    // unused - does not match use case....
}

}
