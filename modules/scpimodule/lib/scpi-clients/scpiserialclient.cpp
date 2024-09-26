#include <QtSerialPort/QSerialPort>

#include <QDebug>

#include "scpiinterface.h"
#include "ieee488-2.h"
#include "scpiserialclient.h"

namespace SCPIMODULE

{

cSCPISerialClient::cSCPISerialClient(QSerialPort* serial, cSCPIModule *module, cSCPIModuleConfigData &configdata, cSCPIInterface *iface) :
    cSCPIClient(module, configdata, iface),
    m_pSerialPort(serial)
{
    qInfo("Serial SCPI connection established");
    connect(m_pSerialPort, &QSerialPort::readyRead, this, &cSCPISerialClient::cmdInput);
}


cSCPISerialClient::~cSCPISerialClient()
{
    qInfo("Serial SCPI connection removed");
}


void cSCPISerialClient::receiveAnswer(QString answ, bool ok)
{
    QString answer;
    QByteArray ba;
    answer = answ + endChar;
    answer.replace("\n", endChar);

    ba = answer.toLatin1();
    m_pSerialPort->write(ba.data(), ba.size());
    qInfo("Serial SCPI command response : %s", qPrintable(answ));
    if(ok)
        emit commandAnswered(this);
}


void cSCPISerialClient::cmdInput()
{
    QString totalInput = m_pSerialPort->readAll();
    qInfo("Serial SCPI command input: %s", qPrintable(makeBareScpiInPrintable(totalInput)));
    m_sInputFifo.append(totalInput);
    testCmd();
}

}

