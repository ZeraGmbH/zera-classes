#include "scpiethclient.h"
#include <QHostAddress>

namespace SCPIMODULE {

cSCPIEthClient::cSCPIEthClient(QTcpSocket *socket, cSCPIModule *module, cSCPIModuleConfigData &configdata, cSCPIInterface *iface) :
    cSCPIClient(module, configdata, iface),
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
    // For now: As soon as sorter comes in this can block sorter forever!!
    if (scpiResponse.isNull())
        return;

    QByteArray ba = scpiResponse.getStr().toUtf8() + "\n";
    m_pSocket->write(ba);
    qInfo("Network SCPI command response : %s", logType == LOG_SKIP ? "<skipped>" : qPrintable(scpiResponse.getStr()));
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
