#include <QtSerialPort/QSerialPort>

#include "scpiinterface.h"
#include "ieee488-2.h"
#include "scpiserialclient.h"

namespace SCPIMODULE

{

cSCPISerialClient::cSCPISerialClient(QSerialPort* serial, VeinPeer *peer, cSCPIModuleConfigData &configdata, cSCPIInterface *iface)
    :m_pSerial(serial), cSCPIClient(peer, configdata, iface)
{
    // so now we can start our connection
    connect(m_pSerial, SIGNAL(readyRead()), this, SLOT(cmdInput()));
}


cSCPISerialClient::~cSCPISerialClient()
{
}


void cSCPISerialClient::receiveAnswer(QString answ)
{
    QString answer;
    QByteArray ba;
    answer = answ +"\n";
    ba = answer.toLatin1();
    m_pSerial->write(ba.data(), ba.size());
}


void cSCPISerialClient::cmdInput()
{

    QString m_sInput;

    while ( m_pSerial->canReadLine() )
    {
        m_sInput = m_pSerial->readLine();
        m_sInput.remove('\n'); // we remove f
        m_sInput.remove('\r');

        if (!m_pSCPIInterface->executeCmd(this, m_sInput))
            emit m_pIEEE4882->AddEventError(CommandError);
    }
}

}

