#include <QtSerialPort/QSerialPort>

#include <QDebug>

#include "scpiinterface.h"
#include "ieee488-2.h"
#include "scpiserialclient.h"

namespace SCPIMODULE

{

cSCPISerialClient::cSCPISerialClient(QSerialPort* serial, cSCPIModule *module) :
    cSCPIClient(module),
    m_pSerialPort(serial)
{
    qInfo("Serial SCPI connection established");
    connect(m_pSerialPort, &QSerialPort::readyRead, this, &cSCPISerialClient::cmdInput);
}


cSCPISerialClient::~cSCPISerialClient()
{
    qInfo("Serial SCPI connection removed");
}


void cSCPISerialClient::handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType)
{
    // For now: As soon as sorter comes in this can block sorter forever!!
    if (scpiResponse.isNull())
        return;

    QString answer = scpiResponse.getStr() + m_endChar;
    answer.replace("\n", m_endChar);

    QByteArray ba = answer.toLatin1();
    m_pSerialPort->write(ba.data(), ba.size());
    qInfo("Serial SCPI command response : %s", logType == LOG_SKIP ? "<skipped>" : qPrintable(scpiResponse.getStr()));
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

