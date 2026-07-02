#ifndef INTERFACEINTERFACE_H
#define INTERFACEINTERFACE_H

#include "scpigroupbase.h"
#include "scpiclient.h"

namespace SCPIMODULE
{

class ScpiGroupDevIface : public ScpiGroupBase
{
    Q_OBJECT
public:
    ScpiGroupDevIface(cSCPIModule* module, cSCPIInterface* iface);
    bool setupScpi();

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient* client, int cmdCode, const QString& sInput, const ScpiTransactionId &scpiTransactionId);
private:
    QString getDevIface();

    cSCPIModule* m_pModule = nullptr;
    QString m_devIfaceCache;
};

}

#endif // INTERFACEINTERFACE_H
