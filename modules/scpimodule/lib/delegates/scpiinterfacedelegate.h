#ifndef SCPIINTERFACEDELEGATE_H
#define SCPIINTERFACEDELEGATE_H

#include "scpibasedelegate.h"
#include "scpiclient.h"

namespace SCPIMODULE {

class cSCPIInterfaceDelegate : public ScpiBaseDelegate
{
    Q_OBJECT
public:
    cSCPIInterfaceDelegate(const QString &cmdParent, const QString &cmd, quint8 type, quint16 cmdCode, const QString &cmdDescription = QString());
    virtual void executeSCPI(cSCPIClient *client, const QString& scpi) override;
signals:
    void signalExecuteSCPI(cSCPIClient* client, int cmdCode, const QString &scpi);
private:
    quint16 m_nCmdCode;
};

typedef std::shared_ptr<cSCPIInterfaceDelegate> cSCPIInterfaceDelegatePtr;

}

#endif // SCPIINTERFACEDELEGATE_H
