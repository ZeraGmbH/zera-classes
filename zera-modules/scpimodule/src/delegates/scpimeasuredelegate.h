#ifndef SCPIMEASUREDELEGATE_H
#define SCPIMEASUREDELEGATE_H

#include <QObject>
#include <QList>

#include "scpidelegate.h"

class QString;
class cSCPI;

namespace SCPIMODULE
{

class cSCPIMeasure;

class cSCPIMeasureDelegate: public cSCPIDelegate
{
   Q_OBJECT

public:
    cSCPIMeasureDelegate(QString cmdParent, QString cmd, quint8 type, quint8 measCode, cSCPIMeasure* scpimeasureobject);
    cSCPIMeasureDelegate(const cSCPIMeasureDelegate& delegate, QHash<cSCPIMeasure*, cSCPIMeasure*>& scpiMeasureTranslationHash);
    virtual bool executeSCPI(cSCPIClient *client, QString& sInput) override;
    virtual bool executeClient(cSCPIClient *client);
    void addscpimeasureObject(cSCPIMeasure* measureobject);

signals:
    void signalStatus(quint8);
    void signalAnswer(QString);

private slots:
    void receiveDone();
    void receiveAnswer(QString s);

private:
    quint8 m_nMeasCode;
    int m_nPending;
    QString m_sAnswer;
    cSCPIClient *m_pClient;
    QList<cSCPIMeasure*> m_scpimeasureObjectList;
};

}

#endif // SCPIMEASUREDELEGATE_H
