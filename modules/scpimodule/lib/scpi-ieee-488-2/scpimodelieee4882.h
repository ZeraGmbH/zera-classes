#ifndef SCPIMODELIEEE4882_H
#define SCPIMODELIEEE4882_H

#include "scpimodelbase.h"
#include "scpitransactionid.h"

namespace SCPIMODULE
{

class ScpiModelIEEE4882 : public ScpiModelBase
{
    Q_OBJECT
public:
    ScpiModelIEEE4882(cSCPIInterface* iface);
    bool setupScpi();

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient* client,
                    int cmdCode,
                    const QString& scpi,
                    const ScpiTransactionId &scpiTransactionId);
};

}

#endif // SCPIMODELIEEE4882_H
