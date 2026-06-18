#include "scpiclientinfo.h"
#include "scpiclient.h"

namespace SCPIMODULE
{

cSCPIClientInfo::cSCPIClientInfo(cSCPIClient *client, int entityid, int parcmdtype, const ScpiTransactionId &scpiTransactionId) :
    m_pClient(client),
    m_nEntityId(entityid),
    m_nParCmdType(parcmdtype),
    m_scpiTransactionId(scpiTransactionId)
{
}

int cSCPIClientInfo::entityId()
{
    return m_nEntityId;
}

int cSCPIClientInfo::parCmdType()
{
    return m_nParCmdType;
}

cSCPIClient *cSCPIClientInfo::getClient()
{
    return m_pClient;
}

ScpiTransactionId cSCPIClientInfo::getScpiTransactionId()
{
    return m_scpiTransactionId;
}

}
