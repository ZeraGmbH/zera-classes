#include "scpitestclient.h"
#include "scpimodule.h"
#include <timemachineobject.h>

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
    m_allResponsesPending += execPendingCmds();
}

QString ScpiTestClient::sendReceiveNotSorted(const QString &scpi, bool removeLineFeedOnReceive)
{
    clearResponses();
    sendScpiCmds(scpi);
    TimeMachineObject::feedEventLoop();
    QString strResponses;
    for (const NullableString &response : getResponsesNotSorted())
        strResponses.append(responseToStr(response, removeLineFeedOnReceive));
    return strResponses;
}

QString ScpiTestClient::sendReceiveSorted(const QString &scpi, bool removeLineFeedOnReceive)
{
    clearResponses();
    sendScpiCmds(scpi);
    TimeMachineObject::feedEventLoop();
    QString strResponses;
    for (const NullableString &response : getResponsesSorted())
        strResponses.append(responseToStr(response, removeLineFeedOnReceive));
    return strResponses;
}

const NullableStringList &ScpiTestClient::getResponsesNotSorted() const
{
    return m_responseNotSorted;
}

const NullableStringList &ScpiTestClient::getResponsesSorted() const
{
    return m_responsesSorted;
}

int ScpiTestClient::getAllHandledResponseCount() const
{
    return m_allResponsesReceivedNotClearable;
}

int ScpiTestClient::getUnhandledResponses() const
{
    return m_allResponsesPending;
}

bool ScpiTestClient::getAtLeastOneResponse() const
{
    return m_allResponsesReceivedNotClearable > 0;
}

void ScpiTestClient::clearResponses()
{
    m_responseNotSorted.clear();
    m_responsesSorted.clear();
}

void ScpiTestClient::handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType)
{
    Q_UNUSED(logType)

    m_responseNotSorted.append(scpiResponse);
    m_allResponsesReceivedNotClearable++;
    m_allResponsesPending--;
    emit sigScpiResponseNotSorted(scpiResponse.getStr(), scpiResponse.isNull(), scpiTransactionId.getScpi());

    const ScpiResponseSorter::SortedResponseList sortedResponses = m_responseSorter.genOrDelaySortedOutput(scpiResponse, scpiTransactionId);
    for (const ScpiResponseSorter::SortedResponse &response : sortedResponses) {
        const NullableString &sortedResponse = response.scpiResponse;
        m_responsesSorted.append(sortedResponse);
        emit sigScpiResponseSorted(sortedResponse.getStr(), sortedResponse.isNull(), response.scpiTransactionId.getScpi());
    }
}

void ScpiTestClient::cmdInput()
{
    // unused - does not match use case....
}

QString ScpiTestClient::responseToStr(const NullableString &response, bool removeLineFeedOnReceive)
{
    if (response.isNull())
        return QString();

    QString strResponse = response.getStr();
    if (removeLineFeedOnReceive)
        strResponse.remove("\n");
    return strResponse;
}

}
