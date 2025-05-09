#ifndef SCPIINTERFACEDELEGATE_H
#define SCPIINTERFACEDELEGATE_H

#include "scpibasedelegate.h"
#include "scpiclient.h"

namespace SCPIMODULE {

class cSCPIInterfaceDelegate : public ScpiBaseDelegate
{
    Q_OBJECT
public:
    cSCPIInterfaceDelegate(QString cmdParent, QString cmd, quint8 type, quint16 cmdCode, QString cmdDescription = QString());
    virtual void executeSCPI(cSCPIClient *client, QString& sInput) override;
signals:
    void signalExecuteSCPI(cSCPIClient* client, int cmdCode, const QString &sInput);
private:
    quint16 m_nCmdCode;
};

typedef std::shared_ptr<cSCPIInterfaceDelegate> cSCPIInterfaceDelegatePtr;

}

#endif // SCPIINTERFACEDELEGATE_H
