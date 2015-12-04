#ifndef SCPISTATUSDELEGATE_H
#define SCPISTATUSDELEGATE_H

#include <QObject>
#include <QString>

#include "scpidelegate.h"

namespace SCPIMODULE
{

class cSCPIClient;

class cSCPIStatusDelegate: public cSCPIDelegate
{
    Q_OBJECT

public:
    cSCPIStatusDelegate(QString cmdParent, QString cmd, quint8 type, quint8 cmdCode, quint8 statindex);
    virtual bool executeSCPI(cSCPIClient *client, const QString& sInput);

signals:
    void executeSCPI(cSCPIClient* client, int cmdCode, int statIndex, const QString &sInput);

private:
    quint8 m_nCmdCode;
    quint8 m_nStatusIndex;
};

}



#endif // SCPISTATUSDELEGATE_H
