#ifndef SCPISTATUSDELEGATE_H
#define SCPISTATUSDELEGATE_H

#include "scpibasedelegate.h"
#include "scpiclient.h"

namespace SCPIMODULE {

class cSCPIStatusDelegate : public ScpiBaseDelegate
{
    Q_OBJECT
public:
    cSCPIStatusDelegate(QString cmdParent, QString cmd, quint8 type, quint8 cmdCode, quint8 statindex);
    virtual void executeSCPI(cSCPIClient *client, QString& sInput) override;
signals:
    void signalExecuteSCPI(cSCPIClient* client, int cmdCode, int statIndex, const QString &sInput);
private:
    quint8 m_nCmdCode;
    quint8 m_nStatusIndex;
};

typedef std::shared_ptr<cSCPIStatusDelegate> cSCPIStatusDelegatePtr;

}

#endif // SCPISTATUSDELEGATE_H
