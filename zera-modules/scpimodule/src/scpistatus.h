#ifndef SCPISTATUS_H
#define SCPISTATUS_H

#include <QObject>
#include <QString>

namespace SCPIMODULE
{

namespace SCPIStatusSystem {
    enum scpiStatusSystem {questionable, operation, operationmeasure}; // we have 1 basic and 1 cascaded status system
}


namespace SCPIStatusCmd {
    enum scpiStatusCmd {condition, ptransition, ntransition, event, enable};
}

const quint8 OperationMeasureSummary = 4; // !!!! bit position

class cSCPIClient;

class cSCPIStatus: public QObject
{
    Q_OBJECT

public:
    cSCPIStatus(quint8 tothrow );

    void executeCmd(cSCPIClient* client, int cmdCode, const QString &sInput);

    quint16 m_nCondition; // condition reg.
    quint16 m_nPTransition; // pos. transition reg.
    quint16 m_nNTransition; // neg. transition reg.
    quint16 m_nEvent; // event reg.
    quint16 m_nEnable; // enable register

signals:
    void event(quint8, quint8); // status object can send an event with 16bit mask
    void eventError(int);

public slots:
    void setCondition(quint16 condition); // we can set our condition reg.
    void SetConditionBit(quint8 bitpos, quint8 val);

private:
    quint8 m_n2Throw; // the bit position

    void readwriteStatusReg(cSCPIClient* client, quint16 &status, QString input);
    void readStatusReg(cSCPIClient* client, quint16 &status, QString input);

};

}

#endif // SCPISTATUS_H
