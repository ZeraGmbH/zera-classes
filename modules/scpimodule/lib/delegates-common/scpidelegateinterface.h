#ifndef SCPIDELEGATEINTERFACE_H
#define SCPIDELEGATEINTERFACE_H

#include "scpidelegatetemplate.h"
#include "scpiclient.h"

namespace SCPIMODULE {

class ScpiDelegateInterface : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    ScpiDelegateInterface(const QString &cmdParent, const QString &cmd,
                           quint8 scpiCmdQueryFlags,
                           quint16 cmdCode,
                           const QString &cmdDescription = QString());
    void executeSCPI(cSCPIClient *client,
                     const QString& scpi,
                     const ScpiTransactionId &scpiTransactionId) override;
signals:
    void signalExecuteSCPI(SCPIMODULE::cSCPIClient* client,
                           int cmdCode,
                           const QString &scpi,
                           const ScpiTransactionId &scpiTransactionId);
private:
    quint16 m_nCmdCode;
};

typedef std::shared_ptr<ScpiDelegateInterface> cSCPIInterfaceDelegatePtr;

}

#endif // SCPIDELEGATEINTERFACE_H
