#include "scpiinterface.h"
#include "ieee488-2.h"
#include "scpiethclient.h"
#include <QTcpSocket>
#include <QHostAddress>

namespace SCPIMODULE

{

cSCPIEthClient::cSCPIEthClient(QTcpSocket *socket, cSCPIModule *module, cSCPIModuleConfigData &configdata, cSCPIInterface *iface)
    :m_pSocket(socket), cSCPIClient(module, configdata, iface)
{
    m_peerAddress = getPeerAddress();
    qInfo("Incoming SCPI socket from %s", qPrintable(m_peerAddress));
    // so now we can start our connection
    connect(m_pSocket, &QTcpSocket::readyRead, this, &cSCPIEthClient::cmdInput);
    connect(m_pSocket, &QTcpSocket::disconnected, this, &cSCPIEthClient::deleteLater);
}


cSCPIEthClient::~cSCPIEthClient()
{
    qInfo("SCPI socket %s closed", qPrintable(m_peerAddress));
    m_pSocket->abort();
    delete m_pSocket;
}


void cSCPIEthClient::receiveAnswer(QString answ)
{
    QString answer;
    QByteArray ba;
    answer = answ +"\n";
    ba = answer.toLatin1();
    m_pSocket->write(ba);
}

QString cSCPIEthClient::getPeerAddress()
{
    QHostAddress addr = m_pSocket->peerAddress();
    return addr.toString();
}


void cSCPIEthClient::cmdInput()
{

    QString m_sInput;

    while (m_pSocket->canReadLine())
    {
        m_sInput = m_pSocket->readLine();
        m_sInputFifo.append(m_sInput);
    }

    testCmd();
}

}
