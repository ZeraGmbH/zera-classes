#ifndef SCPIINTERFACEDELEGATE_H
#define SCPIINTERFACEDELEGATE_H

#include "scpidelegate.h"

namespace SCPIMODULE
{

class cSCPIClient;

class cSCPIInterfaceDelegate: public cSCPIDelegate
{
    Q_OBJECT

public:
    cSCPIInterfaceDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode);
    virtual bool executeSCPI(const QString&, QString&);
    virtual bool executeSCPI(cSCPIClient *client, const QString& sInput);

signals:
    void executeSCPI(cSCPIClient* client, int cmdCode, QString &sInput);

private:
    quint16 m_nCmdCode;
};

}

#endif // SCPIINTERFACEDELEGATE_H
