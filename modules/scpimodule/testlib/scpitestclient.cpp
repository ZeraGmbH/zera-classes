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

void ScpiTestClient::handleCmdFinish(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType)
{
    Q_UNUSED(scpiTransactionId)
    Q_UNUSED(logType)
    emit sigScpiAnswer(scpiResponse);
}

void ScpiTestClient::cmdInput()
{
    // unused - does not match use case....
}

}
