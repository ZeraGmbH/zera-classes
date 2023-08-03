#ifndef SCPIMEASURECOLLECTOR_H
#define SCPIMEASURECOLLECTOR_H

#include <QObject>
#include <QString>

namespace SCPIMODULE
{

class cSCPIClient;

class cSCPIMeasureCollector: public QObject
{
    Q_OBJECT

public:
    cSCPIMeasureCollector(cSCPIClient* client, quint32 nr);

public slots:
    void receiveStatus(quint8 stat);
    void receiveAnswer(QString s);

signals:
    void signalStatus(quint8);

private:
    cSCPIClient* m_pClient;
    quint32 m_nmeasureObjects;
    quint32 m_nmeasureObjectsInit;
    quint8 m_nStatus;
    QString m_sAnswer;
    QMetaObject::Connection myStatusConnection;
    QMetaObject::Connection myAnswerConnection;
};

}

#endif // SCPIMEASURECOLLECTOR_H
