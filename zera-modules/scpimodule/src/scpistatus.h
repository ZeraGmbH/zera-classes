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

const quint16 OperationMeasureSummary = 0x10;

class cSCPIStatus: public QObject
{
    Q_OBJECT

public:
    cSCPIStatus(quint16 tothrow );

    quint16 m_nCondition; // condition reg.
    quint16 m_nPTransition; // pos. transition reg.
    quint16 m_nNTransition; // neg. transition reg.
    quint16 m_nEvent; // event reg.
    quint16 m_nEnable; // enable register

signals:
    void event(quint16); // status object can send an event with 16bit mask

public slots:
    void setCondition(quint16 condition); // we can set our condition reg.

private:
    quint16 m_n2Throw;


};

}

#endif // SCPISTATUS_H
