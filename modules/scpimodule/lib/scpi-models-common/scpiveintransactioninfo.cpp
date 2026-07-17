#include "scpiveintransactioninfo.h"
#include "scpiclient.h"

namespace SCPIMODULE
{

ScpiVeinTransactionInfo::ScpiVeinTransactionInfo(cSCPIClient *client,
                                                 int entityid,
                                                 QueryCmdType queryCmdType,
                                                 const ScpiTransactionId &scpiTransactionId) :
    m_pClient(client),
    m_nEntityId(entityid),
    m_queryCmdType(queryCmdType),
    m_scpiTransactionId(scpiTransactionId)
{
}

int ScpiVeinTransactionInfo::getEntityId()
{
    return m_nEntityId;
}

QueryCmdType ScpiVeinTransactionInfo::getQueryCmdType()
{
    return m_queryCmdType;
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
