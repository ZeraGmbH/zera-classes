#ifndef SCPIGROUPIEEE4882_H
#define SCPIGROUPIEEE4882_H

#include "scpigroupbase.h"
#include "scpitransactionid.h"

namespace SCPIMODULE
{

class ScpiGroupIEEE4882: public ScpiGroupBase
{
    Q_OBJECT
public:
    ScpiGroupIEEE4882(cSCPIInterface* iface);
    bool setupScpi();

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient* client, int cmdCode, const QString& sInput, const ScpiTransactionId &scpiTransactionId);
};

}

#endif // SCPIGROUPIEEE4882_H
