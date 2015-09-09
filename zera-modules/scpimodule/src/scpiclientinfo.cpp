#include "scpiclientinfo.h"

namespace SCPIMODULE
{

cSCPIClientInfo::cSCPIClientInfo(cSCPIClient *client, int entityid)
    :m_pClient(client), m_nEntityId(entityid)
{
}


int cSCPIClientInfo::entityId()
{
    return m_nEntityId;
}


cSCPIClient *cSCPIClientInfo::getClient()
{
    return m_pClient;
}

}
