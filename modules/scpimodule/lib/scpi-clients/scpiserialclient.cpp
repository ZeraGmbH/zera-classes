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


void cSCPISerialClient::handleCmdFinish(const QString &answ, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType)
{
    // for now
    if (answ.isEmpty())
        return;

    QString answer;
    QByteArray ba;
    answer = answ + m_endChar;
    answer.replace("\n", m_endChar);

    ba = answer.toLatin1();
    m_pSerialPort->write(ba.data(), ba.size());
    qInfo("Serial SCPI command response : %s", logType == LOG_SKIP ? "<skipped>" : qPrintable(answ));
}


void cSCPISerialClient::cmdInput()
{
    QString totalInput = m_pSerialPort->readAll();
    m_sInputFifo.append(totalInput);

    QString inputCopy = m_sInputFifo;
    QString singleCmd;
    for(int ch=0; ch<inputCopy.size(); ch++) {
        const QString currentChar(inputCopy[ch]);
        singleCmd += currentChar;
        if(currentChar == "\n" || currentChar == "\r") {
            qInfo("Serial SCPI command input: %s", qPrintable(makeBareScpiInPrintable(singleCmd)));
            singleCmd.clear();
        }
    }
    execPendingCmds();
}

}

