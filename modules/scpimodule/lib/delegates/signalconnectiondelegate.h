#ifndef SIGNALCONNECTIONDELEGATE_H
#define SIGNALCONNECTIONDELEGATE_H

#include "scpistatus.h"
#include <QVariant>

namespace SCPIMODULE {

class cSignalConnectionDelegate: public QObject
{
    Q_OBJECT
public:
    cSignalConnectionDelegate(cSCPIStatus* scpiStatus, quint8 bitnr, int entityid, const QString &componentName);
    int EntityId();
    QString ComponentName();
public slots:
    void setStatus(const QVariant &signal);
private:
    cSCPIStatus* m_pSCPIStatus;
    quint8 m_nBitNr;
    int m_nEntityId;
    QString m_sComponentName;
};

}


#endif // SIGNALCONNECTIONDELEGATE_H
