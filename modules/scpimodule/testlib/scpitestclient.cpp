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
    m_unhandledResponses += execPendingCmds();
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

void ScpiTestClient::clearResponses()
{
    m_responseNotSorted.clear();
    m_responsesSorted.clear();
}

void ScpiTestClient::handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType)
{
    Q_UNUSED(logType)

    m_responseNotSorted.append(scpiResponse);
    m_handledResponses++;
    m_unhandledResponses--;
    emit sigScpiResponseNotSorted(scpiResponse.getStr(), scpiResponse.isNull(), scpiTransactionId.getScpi());

    const NullableStringList sortedResponses = m_responseSorter.genOrDelaySortedOutput(scpiResponse, scpiTransactionId);
    for (const NullableString &response : sortedResponses) {
        m_responsesSorted.append(response);
        emit sigScpiResponseSorted(response.getStr(), response.isNull(), scpiTransactionId.getScpi());
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
