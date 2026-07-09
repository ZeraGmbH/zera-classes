#ifndef SCPIBASEDELEGATE_H
#define SCPIBASEDELEGATE_H

#include "scpiobject.h"
#include "scpitransactionid.h"
#include <scpi.h>
#include <QString>
#include <memory>

namespace SCPIMODULE {

class cSCPIClient;

class ScpiBaseDelegate;
typedef std::shared_ptr<ScpiBaseDelegate> ScpiBaseDelegatePtr;

/* Delegates are ScpiObjects - which are nodes in SCPI tree
 * that can execute commands/queries - see executeSCPI
 */
class ScpiBaseDelegate : public QObject, public ScpiObject
{
   Q_OBJECT
public:
    ScpiBaseDelegate();
    ScpiBaseDelegate(const QString &cmdParent, const QString &cmd, quint8 scpiCmdQueryFlags);
    virtual ~ScpiBaseDelegate();

    const QString &getCmdParent() const;

    bool executeSCPI(const QString&, QString&) override { return true; } // ScpiObject requires
    virtual void executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId) = 0;

    static int getInstanceCount();

protected:
    QString m_sCmdParent;
private:
    static int m_instanceCount;
};


}
#endif // SCPIBASEDELEGATE_H
