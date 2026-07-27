#include "scpiinterface.h"
#include "scpimodule.h"
#include "ieee488-2.h"
#include "scpiserialclient.h"
#include <QSerialPort>

namespace SCPIMODULE {

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
    bool sortResponses = m_pModule->getSortQueryResponse();
    const ScpiResponseSorter::SortedResponseList responseList =
        sortResponses ?
        m_responseSorter.genOrDelaySortedOutput(scpiResponse, scpiTransactionId, logType) :
        m_responseSorter.genImmediateNotSortedOutput(scpiResponse, scpiTransactionId, logType);

    for (const ScpiResponseSorter::SortedResponse &singleResponse : responseList) {
        const NullableString &currentResponse = singleResponse.scpiResponse;
        if (currentResponse.isNull())
            return;

        QString answer = currentResponse.getStr() + m_endChar;
        answer.replace("\n", m_endChar);

        QByteArray ba = answer.toLatin1();
        m_pSerialPort->write(ba.data(), ba.size());
        qInfo("Serial SCPI command response : %s", logType == LOG_SKIP ? "<skipped>" : qPrintable(currentResponse.getStr()));
    }
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

