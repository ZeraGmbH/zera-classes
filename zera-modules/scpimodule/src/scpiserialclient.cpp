#include <QtSerialPort/QSerialPort>

#include <QDebug>

#include "debug.h"
#include "scpiinterface.h"
#include "ieee488-2.h"
#include "scpiserialclient.h"

namespace SCPIMODULE

{

cSCPISerialClient::cSCPISerialClient(QSerialPort* serial, cSCPIModule *module, cSCPIModuleConfigData &configdata, cSCPIInterface *iface)
    :m_pSerialPort(serial), cSCPIClient(module, configdata, iface)
{
    // so now we can start our connection
    connect(m_pSerialPort, &QSerialPort::readyRead, this, &cSCPISerialClient::cmdInput);
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
    m_pSerialPort->write(ba.data(), ba.size());
}


void cSCPISerialClient::cmdInput()
{
    QString addString;

    // first we read all available input and add it to our input fifo
    addString = QString(m_pSerialPort->readAll().data());
    m_sInputFifo.append(addString);

    testCmd();
}

}

