#ifndef SCPIVEINTRANSACTIONINFO_H
#define SCPIVEINTRANSACTIONINFO_H

#include "scpitransactionid.h"
#include <QString>
#include <memory>

namespace SCPIMODULE
{

class cSCPIClient;

enum parCmdType {parcmd, parQuery};

class ScpiVeinTransactionInfo
{
public:
    ScpiVeinTransactionInfo(cSCPIClient* client, int entityid, int parcmdtype, const ScpiTransactionId &scpiTransactionId);

    int entityId();
    int parCmdType();
    cSCPIClient* getClient();
    ScpiTransactionId getScpiTransactionId();

private:
    cSCPIClient *m_pClient;
    int m_nEntityId;
    int m_nParCmdType;
    ScpiTransactionId m_scpiTransactionId;
};

typedef std::shared_ptr<ScpiVeinTransactionInfo> SCPIVeinTransactionInfoPtr;


}

#endif // SCPIVEINTRANSACTIONINFO_H

