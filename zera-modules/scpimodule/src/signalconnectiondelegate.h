#ifndef SIGNALCONNECTIONDELEGATE_H
#define SIGNALCONNECTIONDELEGATE_H

#include <QObject>
#include <QVariant>

namespace SCPIMODULE
{

class cSCPIStatus;

class cSignalConnectionDelegate: public QObject
{
    Q_OBJECT

public:
    cSignalConnectionDelegate(cSCPIStatus* scpiStatus, quint8 bitnr);

public slots:
    void setStatus(QVariant signal);

private:
    cSCPIStatus* m_pSCPIStatus;
    quint8 m_nBitNr;
};

}


#endif // SIGNALCONNECTIONDELEGATE_H
