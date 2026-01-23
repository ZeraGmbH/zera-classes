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
    ScpiBaseDelegate(const QString &cmdParent, const QString &cmd, quint8 type);
    virtual ~ScpiBaseDelegate();
    void setCommand(cSCPI *scpiCmdInterface, ScpiBaseDelegatePtr delegate);
    bool executeSCPI(const QString&, QString&) override { return true; } // ScpiObject requires
    virtual void executeSCPI(cSCPIClient *client, const QString &scpi) = 0;

    static int getInstanceCount();

protected:
    QString m_sCmdParent;
private:
    static int m_instanceCount;
};


}
#endif // SCPIBASEDELEGATE_H
