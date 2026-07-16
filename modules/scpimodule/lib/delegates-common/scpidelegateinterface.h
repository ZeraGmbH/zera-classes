#ifndef SCPIDELEGATEINTERFACE_H
#define SCPIDELEGATEINTERFACE_H

#include "scpidelegatetemplate.h"
#include "scpiclient.h"

namespace SCPIMODULE {

class ScpiDelegateInterface : public ScpiDelegateTemplate
{
    Q_OBJECT
public:
    struct Params {
        const QString cmdParent;
        const QString cmd;
        const quint8 scpiQueryCmdFlags = 0;
        const quint16 cmdCode = 0;
        const QString cmdDescription; // optional
    };
    ScpiDelegateInterface(const Params &params);
    void executeSCPI(cSCPIClient *client,
                     const QString& scpi,
                     const ScpiTransactionId &scpiTransactionId) override;
signals:
    void signalExecuteSCPI(SCPIMODULE::cSCPIClient* client,
                           int cmdCode,
                           const QString &scpi,
                           const ScpiTransactionId &scpiTransactionId);
private:
    const quint16 m_nCmdCode;
};

typedef std::shared_ptr<ScpiDelegateInterface> cSCPIInterfaceDelegatePtr;

}

#endif // SCPIDELEGATEINTERFACE_H
