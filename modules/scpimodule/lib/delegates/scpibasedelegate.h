#ifndef SCPIBASEDELEGATE_H
#define SCPIBASEDELEGATE_H

#include "scpiobject.h"
#include <scpi.h>
#include <QObject>
#include <QString>
#include <memory>

namespace SCPIMODULE {

class cSCPIClient;

class ScpiBaseDelegate;
typedef std::shared_ptr<ScpiBaseDelegate> ScpiBaseDelegatePtr;

class ScpiBaseDelegate : public QObject, public ScpiObject
{
   Q_OBJECT
public:
    ScpiBaseDelegate();
    ScpiBaseDelegate(QString cmdParent, QString cmd, quint8 type);
    virtual ~ScpiBaseDelegate() = default;
    void setCommand(cSCPI *scpiCmdInterface, ScpiBaseDelegatePtr delegate);
    bool executeSCPI(const QString&, QString&) override { return true; } // ScpiObject requires
    virtual void executeSCPI(cSCPIClient *client, QString& sInput) = 0;

protected:
    QString m_sCmdParent;
};


}
#endif // SCPIBASEDELEGATE_H
