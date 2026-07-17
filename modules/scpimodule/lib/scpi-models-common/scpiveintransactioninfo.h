#ifndef SCPIVEINTRANSACTIONINFO_H
#define SCPIVEINTRANSACTIONINFO_H

#include "scpitransactionid.h"
#include <QString>
#include <memory>

namespace SCPIMODULE
{

class cSCPIClient;

enum QueryCmdType {
    TYPE_CMD,
    TYPE_QUERY
};

class ScpiVeinTransactionInfo
{
public:
    ScpiVeinTransactionInfo(cSCPIClient* client,
                            int entityid,
                            QueryCmdType queryCmdType,
                            const ScpiTransactionId &scpiTransactionId);

    int getEntityId();
    QueryCmdType getQueryCmdType();
    cSCPIClient* getClient();
    ScpiTransactionId getScpiTransactionId();

private:
    cSCPIClient *m_pClient = nullptr;
    int m_nEntityId = 0;
    QueryCmdType m_queryCmdType = TYPE_QUERY;
    ScpiTransactionId m_scpiTransactionId;
};

typedef std::shared_ptr<ScpiVeinTransactionInfo> SCPIVeinTransactionInfoPtr;

}

#endif // SCPIVEINTRANSACTIONINFO_H

