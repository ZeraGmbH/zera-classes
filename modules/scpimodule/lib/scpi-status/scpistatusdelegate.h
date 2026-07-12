#ifndef SCPISTATUSDELEGATE_H
#define SCPISTATUSDELEGATE_H

#include "scpidelegatetemplate.h"
#include "scpiclient.h"

namespace SCPIMODULE {

class cSCPIStatusDelegate : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    cSCPIStatusDelegate(const QString &cmdParent,
                        const QString &cmd,
                        quint8 scpiCmdQueryFlags,
                        SCPIStatusDefinitions::ScpiStatusCommands cmdCode,
                        SCPIStatusDefinitions::ScpiStatusSystems statusSystemIdx);
    void executeSCPI(cSCPIClient *client, const QString& scpi, const ScpiTransactionId &scpiTransactionId) override;
signals:
    void signalExecuteSCPI(SCPIMODULE::cSCPIClient* client,
                           SCPIStatusDefinitions::ScpiStatusCommands cmdCode,
                           SCPIStatusDefinitions::ScpiStatusSystems statusSystemIdx,
                           const QString &scpi,
                           const ScpiTransactionId &scpiTransactionId);

private:
    SCPIStatusDefinitions::ScpiStatusCommands m_cmdCode;
    SCPIStatusDefinitions::ScpiStatusSystems m_statusSystemIdx;
};

typedef std::shared_ptr<cSCPIStatusDelegate> cSCPIStatusDelegatePtr;

}

#endif // SCPISTATUSDELEGATE_H
