#include <QtSerialPort/QSerialPort>

#include <QDebug>

#include "debug.h"
#include "scpiinterface.h"
#include "ieee488-2.h"
#include "scpiserialclient.h"

namespace SCPIMODULE

{

cSCPISerialClient::cSCPISerialClient(QSerialPort* serial, cSCPIModule *module, cSCPIModuleConfigData &configdata, cSCPIInterface *iface)
    :m_pSerial(serial), cSCPIClient(module, configdata, iface)
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

#ifdef DEBUG
    qDebug() << answer.toLatin1();
#endif

    ba = answer.toLatin1();
    m_pSerial->write(ba.data(), ba.size());
}


void cSCPISerialClient::cmdInput()
{
    QString addString;

    // first we read all available input and add it to our input fifo
    addString = QString(m_pSerial->readAll().data());
    m_sInputFifo.append(addString);

    testCmd();
}

}

