#include "scpiethclient.h"
#include "scpimodule.h"
#include <QHostAddress>

namespace SCPIMODULE {

cSCPIEthClient::cSCPIEthClient(QTcpSocket *socket, cSCPIModule *module) :
    cSCPIClient(module),
    m_pSocket(socket)
{
    connect(m_pSocket, &QTcpSocket::readyRead, this, &cSCPIEthClient::cmdInput);
}

cSCPIEthClient::~cSCPIEthClient()
{
    m_pSocket->abort();
    m_pSocket->deleteLater();
}

void cSCPIEthClient::handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType)
{
    bool sortResponses = m_pModule->getSortQueryResponse();
    const ScpiResponseSorter::SortedResponseList responseList =
        sortResponses ?
        m_responseSorter.genOrDelaySortedOutput(scpiResponse, scpiTransactionId, logType) :
        m_responseSorter.genImmediateNotSortedOutput(scpiResponse, scpiTransactionId, logType);

    for (const ScpiResponseSorter::SortedResponse &singleResponse : responseList) {
        const NullableString &currentResponse = singleResponse.scpiResponse;
        if (currentResponse.isNull())
            continue;

        QByteArray ba = currentResponse.getStr().toUtf8() + "\n";
        m_pSocket->write(ba);
        qInfo("Network SCPI command response : %s", logType == LOG_SKIP ? "<skipped>" : qPrintable(currentResponse.getStr()));
    }
}

QString cSCPIEthClient::getPeerAddress()
{
    QHostAddress addr = m_pSocket->peerAddress();
    return addr.toString();
}

void cSCPIEthClient::cmdInput()
{
    QString totalInput;
    while (m_pSocket->canReadLine()) {
        QString inputLine = m_pSocket->readLine();
        m_sInputFifo.append(inputLine);
        totalInput.append(inputLine);
    }
    qInfo("Network SCPI command input: %s", qPrintable(makeBareScpiInPrintable(totalInput)));
    execPendingCmds();
}

}
