#ifndef SCPIMODELIEEE4882_H
#define SCPIMODELIEEE4882_H

#include "scpiinterface.h"
#include "scpimodelbase.h"
#include "scpitransactionid.h"

namespace SCPIMODULE
{

class ScpiModelIEEE4882 : public ScpiModelBase
{
    Q_OBJECT
public:
    void setupScpi(cSCPIInterface* scpiInterface);

private slots:
    void executeCmd(SCPIMODULE::cSCPIClient* client,
                    int cmdCode,
                    const QString& scpi,
                    const ScpiTransactionId &scpiTransactionId);
};

}

#endif // SCPIMODELIEEE4882_H
