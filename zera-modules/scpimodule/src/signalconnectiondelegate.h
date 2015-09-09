#ifndef SIGNALCONNECTIONDELEGATE_H
#define SIGNALCONNECTIONDELEGATE_H

#include <QObject>
#include <QVariant>
#include <QString>

namespace SCPIMODULE
{

class cSCPIStatus;

class cSignalConnectionDelegate: public QObject
{
    Q_OBJECT

public:
    cSignalConnectionDelegate(cSCPIStatus* scpiStatus, quint8 bitnr, int entityid, QString cname);
    int EntityId();
    QString ComponentName();

public slots:
    void setStatus(QVariant signal);

private:
    cSCPIStatus* m_pSCPIStatus;
    quint8 m_nBitNr;
    int m_nEntityId;
    QString m_sComponentName;
};

}


#endif // SIGNALCONNECTIONDELEGATE_H
