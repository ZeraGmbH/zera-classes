#ifndef ADJUSTMENTMODULECOMMON_H
#define ADJUSTMENTMODULECOMMON_H

#include "adjustmentmoduleconfigdata.h"
#include "pcbinterface.h"
#include "proxy.h"
#include <QList>
#include <QHash>

namespace ADJUSTMENTMODULE
{
class cAdjustmentModule;

struct AdjustmentServersAndConfig
{
    Zera::Server::cPCBInterface *pcbInterface;
    Zera::Server::cPCBInterface *m_AdjustPCBInterface;
    std::function<void(void)> setInterfaceValidation;
};

class cAdjustChannelInfo;

struct AdjustmentModuleActivateData
{
    QList<Zera::Proxy::cProxyClient*> m_pcbClientList; // a list with our pcb clients ... for clean up
    QHash<QString,int> m_chnPortHash; // a hash for our channels ethernet ports
    QHash<QString, cAdjustChannelInfo*> m_adjustChannelInfoHash;
    QHash<int, QString> m_portChannelHash; // a list of ports for which we have established connection
    QList<Zera::Server::cPCBInterface*> m_pcbInterfaceList; // a list of pcbinterfaces ... for clean up
    QHash<QString, QString> m_AliasChannelHash; // we use this hash for easier access
    bool m_bAuthorized;
};
}
#endif // ADJUSTMENTMODULECOMMON_H
