#ifndef SCPIBASEDELEGATE_H
#define SCPIBASEDELEGATE_H

#include "scpiobject.h"
#include <scpi.h>
#include <QObject>
#include <QString>

namespace SCPIMODULE {

class cSCPIClient;

class ScpiBaseDelegate: public QObject, public cSCPIObject
{
   Q_OBJECT
public:
    ScpiBaseDelegate();
    ScpiBaseDelegate(QString cmdParent, QString cmd, quint8 type);
    virtual ~ScpiBaseDelegate() = default;
    void setCommand(cSCPI *scpiCmdInterface);
    bool executeSCPI(const QString&, QString&) override { return true; } // cSCPIObject requires
    virtual bool executeSCPI(cSCPIClient *client, QString& sInput) = 0;
signals:
    void signalStatus(quint8);
protected:
    QString m_sCmdParent;
};

}
#endif // SCPIBASEDELEGATE_H
