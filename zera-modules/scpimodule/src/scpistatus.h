#ifndef SCPISTATUS_H
#define SCPISTATUS_H

#include <QObject>
#include <QString>

class cSCPIStatus: public QObject
{
    Q_OBJECT

public:
    cSCPIStatus();
    cSCPIStatus(QString cmdparent, QString cmd );

signals:
    void event(quint16); // status object can send an event with 16bit mask

public slots:
    void setCondition(quint16); // we can set our condition reg.

private:
    quint16 m_nCondition; // condition reg.
    quint16 m_nPTransition; // pos. transition reg.
    quint16 m_nNTransition; // neg. transition reg.
    quint16 m_nEvent; // event reg.
    quint16 m_nEnable;  // enable register
};

#endif // SCPISTATUS_H
