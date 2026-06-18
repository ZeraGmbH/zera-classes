#ifndef SCPICLIENTINFO
#define SCPICLIENTINFO

#include "scpitransactionid.h"
#include <QString>
#include <memory>

namespace SCPIMODULE
{

class cSCPIClient;

enum parCmdType {parcmd, parQuery};

class cSCPIClientInfo
{
public:
    cSCPIClientInfo(cSCPIClient* client, int entityid, int parcmdtype, const ScpiTransactionId &scpiTransactionId);

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

typedef std::shared_ptr<cSCPIClientInfo> SCPIClientInfoPtr;


}

#endif // SCPICLIENTINFO

