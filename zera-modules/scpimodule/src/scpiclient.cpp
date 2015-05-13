#include <QTcpSocket>
#include <scpi.h>

#include "ieee488-2.h"
#include "scpiclient.h"
#include "scpiinterface.h"
#include "scpistatus.h"


namespace SCPIMODULE
{

cSCPIClient::cSCPIClient(QTcpSocket *socket, cSCPIInterface *iface)
    :m_pSocket(socket), m_pSCPIInterface(iface)
{
    m_bAuthorisation = false;

    // we instantiate 3 scpi status systems per client
    cSCPIStatus* scpistatus;
    cSCPIStatus* scpi2status;

    scpistatus = new cSCPIStatus(STBQUES);
    m_SCPIStatusList.append(scpistatus);
    scpistatus = new cSCPIStatus(STBOPER);
    m_SCPIStatusList.append(scpistatus);
    scpi2status = new cSCPIStatus(OperationMeasureSummary);
    m_SCPIStatusList.append(scpistatus);

    connect(scpi2status, SIGNAL(event(quint16)), scpistatus, SLOT(setCondition(quint16)));

    connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(cmdInput()));
    connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}


cSCPIClient::~cSCPIClient()
{
    m_pSocket->abort();
}


void cSCPIClient::setAuthorisation(bool auth)
{
    m_bAuthorisation = auth;
}


cSCPIStatus *cSCPIClient::getSCPIStatus(quint8 index)
{
    return m_SCPIStatusList.at(index);
}


void cSCPIClient::cmdInput()
{
    QString m_sInput;

    m_sInput = "";
    while ( m_pSocket->canReadLine() )
        m_sInput += m_pSocket->readLine();

    m_sInput.remove('\r'); // we remove cr lf
    m_sInput.remove('\n');

    m_pSCPIInterface->executeCmd(this, m_sInput);

    // QStringList cmds = m_sInput.split(';');

    //for (int i = 0; i < cmds.count(); i++)
        //m_pSCPIInterface->executeCmd(cmds.at(i));
}


void cSCPIClient::receiveStatus(quint8 stat)
{
    QString answer;
    QByteArray ba;
    answer = QString("%1\n").arg(SCPI::scpiAnswer[stat]);
    ba = answer.toLatin1();
    m_pSocket->write(ba);
}


void cSCPIClient::receiveAnswer(QString answ)
{
    QString answer;
    QByteArray ba;
    answer = answ +"\n";
    ba = answer.toLatin1();
    m_pSocket->write(ba);
}

}
