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

const NullableString &ScpiTestClient::getLastResponse() const
{
    return m_lastResponse;
}

int ScpiTestClient::getUnhandledResponses() const
{
    return m_unhandledResponses;
}

bool ScpiTestClient::getAtLeastOneResponse() const
{
    return m_atLeastOneResponse;
}

void ScpiTestClient::handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType)
{
    Q_UNUSED(logType)

    m_atLeastOneResponse = true;
    m_lastResponse = scpiResponse;
    m_unhandledResponses--;

    const NullableStringList sortedResponses = m_responseSorter.genOrDelaySortedOutput(scpiResponse, scpiTransactionId);
    for (const NullableString &response : sortedResponses)
        emit sigScpiAnswer(response.getStr(), response.isNull());
}

void ScpiTestClient::cmdInput()
{
    // unused - does not match use case....
}

}
