#ifndef SCPIDELEGATESTATUS_H
#define SCPIDELEGATESTATUS_H

#include "scpidelegatetemplate.h"
#include "scpiclient.h"

namespace SCPIMODULE {

class ScpiDelegateStatus : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    struct Params {
        const QString cmdParent;
        const QString cmd;
        const quint8 scpiCmdQueryFlags = 0;
        const SCPIStatusDefinitions::ScpiStatusCommands cmdCode;
        const SCPIStatusDefinitions::ScpiStatusSystems statusSystemIdx;
    };
    ScpiDelegateStatus(const Params &params);
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

typedef std::shared_ptr<ScpiDelegateStatus> cSCPIStatusDelegatePtr;

}

#endif // SCPIDELEGATESTATUS_H
