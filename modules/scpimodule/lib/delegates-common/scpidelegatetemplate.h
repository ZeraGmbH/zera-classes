#ifndef SCPIDELEGATETEMPLATE_H
#define SCPIDELEGATETEMPLATE_H

#include "scpiobject.h"
#include "scpitransactionid.h"
#include <scpi.h>
#include <QStringList>
#include <memory>

namespace SCPIMODULE {

class cSCPIClient;

class ScpiDelegateTemplate;
typedef std::shared_ptr<ScpiDelegateTemplate> ScpiBaseDelegatePtr;

/* Delegates are ScpiObjects - which are nodes in SCPI tree
 * that can execute commands/queries - see executeSCPI
 */
class ScpiDelegateTemplate : public ScpiObject
{
   Q_OBJECT
public:
    ScpiDelegateTemplate();
    ScpiDelegateTemplate(const QString &cmdParent, const QString &cmd, quint8 scpiCmdQueryFlags);
    virtual ~ScpiDelegateTemplate();

    const QStringList &getCmdParent() const;

    bool executeSCPI(const QString&, QString&) override { return true; } // ScpiObject requires
    virtual void executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId) = 0;

    static int getInstanceCount();

protected:
    const QStringList m_cmdParent;
private:
    static int m_instanceCount;
};


}
#endif // SCPIDELEGATETEMPLATE_H
