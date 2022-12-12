#ifndef ADJUSTMENTMODULECOMMON_H
#define ADJUSTMENTMODULECOMMON_H

#include "adjustmentmoduleconfigdata.h"
#include "pcbinterface.h"
#include "proxy.h"
#include <QList>
#include <QHash>
#include <memory>

namespace ADJUSTMENTMODULE {

class cAdjustChannelInfo;

struct AdjustmentModuleActivateData
{
    QHash<QString, cAdjustChannelInfo*> m_adjustChannelInfoHash;
    QHash<QString, QString> m_AliasChannelHash; // we use this hash for easier access
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    Zera::Proxy::ProxyClientPtr m_pcbClient;
};

typedef std::shared_ptr<AdjustmentModuleActivateData> AdjustmentModuleActivateDataPtr;
}
#endif // ADJUSTMENTMODULECOMMON_H
