#include <QTcpSocket>

#include "scpiinterface.h"
#include "ieee488-2.h"
#include "scpiethclient.h"

namespace SCPIMODULE

{

cSCPIEthClient::cSCPIEthClient(QTcpSocket *socket, cSCPIModule *module, cSCPIModuleConfigData &configdata, cSCPIInterface *iface)
    :m_pSocket(socket), cSCPIClient(module, configdata, iface)
{
    // so now we can start our connection
    connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(cmdInput()));
    connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}


cSCPIEthClient::~cSCPIEthClient()
{
    m_pSocket->abort();
}


void cSCPIEthClient::receiveAnswer(QString answ)
{
    QString answer;
    QByteArray ba;
    answer = answ +"\n";
    ba = answer.toLatin1();
    m_pSocket->write(ba);
}


void cSCPIEthClient::cmdInput()
{

    QString m_sInput;

    while ( m_pSocket->canReadLine() )
    {
        m_sInput = m_pSocket->readLine();
        m_sInput.remove('\n'); // we remove f
        m_sInput.remove('\r');

        if (!m_pSCPIInterface->executeCmd(this, m_sInput))
            emit m_pIEEE4882->AddEventError(CommandError);
        if (scpiClientInfoHash.count() > 0)
            break;
    }
}

}
