#include "scpitestclient.h"
#include "scpimodule.h"

namespace SCPIMODULE {

ScpiTestClient::ScpiTestClient(cSCPIModule *module) :
    cSCPIClient(module)
{
    module->getSCPIServer()->addScpiClient(this);
}

ScpiTestClient::~ScpiTestClient()
{
    m_pModule->getSCPIServer()->removeScpiClient(this);
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

const NullableStringList &ScpiTestClient::getResponsesNotSorted() const
{
    return m_responseNotSorted;
}

cSCPIInterface* ScpiTestClient::getScpiInterface()
{
    return m_pSCPIInterface;
}

const NullableStringList &ScpiTestClient::getResponsesSorted() const
{
    return m_responsesSorted;
}

int ScpiTestClient::getHandledResponses() const
{
    return m_handledResponses;
}

int ScpiTestClient::getUnhandledResponses() const
{
    return m_unhandledResponses;
}

bool ScpiTestClient::getAtLeastOneResponse() const
{
    return m_handledResponses > 0;
}

void ScpiTestClient::handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType)
{
    Q_UNUSED(logType)

    m_responseNotSorted.append(scpiResponse);
    m_handledResponses++;
    m_unhandledResponses--;
    emit sigScpiResponseNotSorted(scpiResponse.getStr(), scpiResponse.isNull());

    const NullableStringList sortedResponses = m_responseSorter.genOrDelaySortedOutput(scpiResponse, scpiTransactionId);
    for (const NullableString &response : sortedResponses) {
        m_responsesSorted.append(response);
        emit sigScpiResponseSorted(response.getStr(), response.isNull());
    }
}

void ScpiTestClient::cmdInput()
{
    // unused - does not match use case....
}

}
