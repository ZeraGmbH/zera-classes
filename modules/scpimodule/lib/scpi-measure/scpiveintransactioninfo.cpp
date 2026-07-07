#include "scpiveintransactioninfo.h"
#include "scpiclient.h"

namespace SCPIMODULE
{

ScpiVeinTransactionInfo::ScpiVeinTransactionInfo(cSCPIClient *client, int entityid, int parcmdtype, const ScpiTransactionId &scpiTransactionId) :
    m_pClient(client),
    m_nEntityId(entityid),
    m_nParCmdType(parcmdtype),
    m_scpiTransactionId(scpiTransactionId)
{
}

int ScpiVeinTransactionInfo::entityId()
{
    return m_nEntityId;
}

int ScpiVeinTransactionInfo::parCmdType()
{
    return m_nParCmdType;
}

cSCPIClient *ScpiVeinTransactionInfo::getClient()
{
    return m_pClient;
}

ScpiTransactionId ScpiVeinTransactionInfo::getScpiTransactionId()
{
    return m_scpiTransactionId;
}

}
