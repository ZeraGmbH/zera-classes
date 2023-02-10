#ifndef SCPICLIENTINFO
#define SCPICLIENTINFO

#include <QString>
#include <memory>

namespace SCPIMODULE
{

class cSCPIClient;

enum parCmdType {parcmd, parQuery};

class cSCPIClientInfo
{
public:
    cSCPIClientInfo(cSCPIClient* client, int entityid, int parcmdtype = parcmd);

    int entityId();
    int parCmdType();
    cSCPIClient* getClient();

private:
    cSCPIClient *m_pClient;
    int m_nEntityId;
    int m_nParCmdType;
};

typedef std::shared_ptr<cSCPIClientInfo> SCPIClientInfoPtr;


}

#endif // SCPICLIENTINFO

