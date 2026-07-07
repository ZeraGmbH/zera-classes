#ifndef INTERFACEINTERFACE_H
#define INTERFACEINTERFACE_H

#include "scpimodelbase.h"
#include "scpiclient.h"

namespace SCPIMODULE
{

class ScpiModelDevIface : public ScpiModelBase
{
    Q_OBJECT
public:
    ScpiModelDevIface(cSCPIModule* module, cSCPIInterface* iface);
    bool setupScpi();

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient* client,
                    int cmdCode,
                    const QString& scpi,
                    const ScpiTransactionId &scpiTransactionId);
private:
    QString getDevIface();

    cSCPIModule* m_pModule = nullptr;
    QString m_devIfaceCache;
};

}

#endif // INTERFACEINTERFACE_H
