#include <QtSerialPort/QSerialPort>
#include <QDebug>

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
    answer = answ + endChar;
    answer.replace("\n", endChar);
    ba = answer.toLatin1();
    m_pSerial->write(ba.data(), ba.size());
}


void cSCPISerialClient::cmdInput()
{
    QString addString;

    addString = QString(m_pSerial->readAll().data());
    m_sInputFifo.append(addString);

    qDebug() << addString;

    if (addString.contains("\n") or (addString.contains("\r"))) // we accept cr or lf
    {
        if (addString.contains('\n'))
            endChar = '\n';
        else
            endChar = '\r';

        while (m_sInputFifo.contains(endChar))
        {
            int index;
            QString m_sInput;

            index = m_sInputFifo.indexOf(endChar);
            m_sInput = m_sInputFifo.left(index);
            m_sInputFifo.remove(0, index+1);

            if (!m_pSCPIInterface->executeCmd(this, m_sInput))
                emit m_pIEEE4882->AddEventError(CommandError);
        }
    }
}

}

