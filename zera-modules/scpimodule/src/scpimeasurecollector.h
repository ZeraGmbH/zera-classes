#ifndef SCPIMEASURECOLLECTOR_H
#define SCPIMEASURECOLLECTOR_H

#include <QObject>
#include <QString>

namespace SCPIMODULE
{

class cSCPIMeasureCollector: public QObject
{
    Q_OBJECT

public:
    cSCPIMeasureCollector(quint32 nr);
    ~cSCPIMeasureCollector();

signals:
    void cmdStatus(quint8);
    void cmdAnswer(QString);

public slots:
    void receiveStatus(quint8 stat);
    void receiveAnswer(QString s);

private:
    quint32 m_nmeasureObjects;
    quint8 m_nStatus;
    QString m_sAnswer;
};

}

#endif // SCPIMEASURECOLLECTOR_H
