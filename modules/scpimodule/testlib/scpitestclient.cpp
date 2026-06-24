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
    m_unhandledResponses += execPendingCmds();
}

cSCPIInterface* ScpiTestClient::getScpiInterface()
{
    return m_pSCPIInterface;
}

int ScpiTestClient::getUnhandledResponses() const
{
    return m_unhandledResponses;
}

void ScpiTestClient::handleCmdFinish(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType)
{
    Q_UNUSED(logType)

    m_unhandledResponses--;

    const QStringList sortedResponses = m_responseSorter.genOrDelaySortedOutput(scpiResponse, scpiTransactionId);
    for (const QString &response : sortedResponses)
        // Test client signals empty responses for more detailed analysis of behaviour
        // Production clients might not send out empty responses
        emit sigScpiAnswer(response);
}

void ScpiTestClient::cmdInput()
{
    // unused - does not match use case....
}

}
