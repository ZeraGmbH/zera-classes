#ifndef SCPICLIENTINFO
#define SCPICLIENTINFO

#include <QString>

namespace SCPIMODULE
{

class cSCPIClient;

class cSCPIClientInfo
{
public:
    cSCPIClientInfo(cSCPIClient* client, int entityid);

    int entityId();
    cSCPIClient* getClient();

private:
    cSCPIClient *m_pClient;
    int m_nEntityId;
};

}

#endif // SCPICLIENTINFO

