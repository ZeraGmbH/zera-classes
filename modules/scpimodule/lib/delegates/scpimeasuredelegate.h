#ifndef SCPIMEASUREDELEGATE_H
#define SCPIMEASUREDELEGATE_H

#include "scpibasedelegate.h"
#include "scpimeasure.h"
#include <QList>

namespace SCPIMODULE {

class cSCPIMeasureDelegate: public ScpiBaseDelegate
{
   Q_OBJECT
public:
    cSCPIMeasureDelegate(const QString &cmdParent, const QString &cmd, quint8 type, quint8 measCode, cSCPIMeasure* scpimeasureobject);
    cSCPIMeasureDelegate(const cSCPIMeasureDelegate& delegate,
                         QHash<cSCPIMeasure*, cSCPIMeasure*> &scpiMeasureTranslationHash);
    virtual void executeSCPI(cSCPIClient *client, const QString& scpi) override;
    void executeClient(cSCPIClient *client);
    void addscpimeasureObject(cSCPIMeasure* measureobject);

private slots:
    void receiveDone();
    void receiveAnswer(QString s);

private:
    quint8 m_nMeasCode;
    int m_nPending;
    QString m_sAnswer;
    cSCPIClient *m_pClient = nullptr;
    QList<cSCPIMeasure*> m_scpimeasureObjectList;
};

typedef std::shared_ptr<cSCPIMeasureDelegate> cSCPIMeasureDelegatePtr;

}

#endif // SCPIMEASUREDELEGATE_H
