#ifndef SCPISTATUSDELEGATE_H
#define SCPISTATUSDELEGATE_H

#include "scpibasedelegate.h"
#include "scpiclient.h"

namespace SCPIMODULE {

class cSCPIStatusDelegate : public ScpiBaseDelegate
{
    Q_OBJECT
public:
    cSCPIStatusDelegate(const QString &cmdParent, const QString &cmd, quint8 type, quint8 cmdCode, quint8 statindex);
    virtual void executeSCPI(cSCPIClient *client, const QString& scpi) override;
signals:
    void signalExecuteSCPI(cSCPIClient* client, int cmdCode, int statIndex, const QString &scpi);
private:
    quint8 m_nCmdCode;
    quint8 m_nStatusIndex;
};

typedef std::shared_ptr<cSCPIStatusDelegate> cSCPIStatusDelegatePtr;

}

#endif // SCPISTATUSDELEGATE_H
