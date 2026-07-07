#ifndef SCPISTATUS_H
#define SCPISTATUS_H

#include "scpitransactionid.h"
#include "scpistatusdefinitions.h"
#include <QString>

namespace SCPIMODULE
{

const quint8 OperationMeasureSummary = 4; // !!!! bit position

class cSCPIClient;

class cSCPIStatus: public QObject
{
    Q_OBJECT
public:
    explicit cSCPIStatus(quint8 bitPositionToThrow);
    void executeCmd(cSCPIClient* client,
                    SCPIStatusDefinitions::ScpiStatusCommands cmdCode,
                    const QString &sInput,
                    const ScpiTransactionId &scpiTransactionId);

    quint16 m_nCondition; // condition reg.
    quint16 m_nPTransition; // pos. transition reg.
    quint16 m_nNTransition; // neg. transition reg.
    quint16 m_nEvent; // event reg.
    quint16 m_nEnable; // enable register
signals:
    void sigEvent(quint8, quint8); // status object can send an event with 16bit mask
    void sigEventErrorCmdFinish(int errorNo, const ScpiTransactionId &scpiTransactionId);
public slots:
    void setCondition(quint16 condition); // we can set our condition reg.
    void SetConditionBit(quint8 bitpos, quint8 val);

private:
    void readwriteStatusReg(cSCPIClient* client, quint16 &status, const QString &input, const ScpiTransactionId &scpiTransactionId);
    void readStatusReg(cSCPIClient* client, quint16 &status, const QString &input, const ScpiTransactionId &scpiTransactionId);

    quint8 m_bitPositionToThrow; // the bit position
};

}

#endif // SCPISTATUS_H
