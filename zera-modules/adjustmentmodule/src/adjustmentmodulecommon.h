#ifndef ADJUSTMENTMODULECOMMON_H
#define ADJUSTMENTMODULECOMMON_H

#include "adjustmentmoduleconfigdata.h"
#include "pcbinterface.h"
#include "proxy.h"
#include <QHash>
#include <unordered_map>
#include <memory>

namespace ADJUSTMENTMODULE {

class cAdjustChannelInfo
{
public:
    cAdjustChannelInfo(){}

    QString m_sAlias;
    QStringList m_sRangelist;
};

struct AdjustmentModuleActivateData
{
    std::unordered_map<QString, std::unique_ptr<cAdjustChannelInfo>> m_adjustChannelInfoHash;
    QHash<QString, QString> m_AliasChannelHash; // we use this hash for easier access
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    Zera::Proxy::ProxyClientPtr m_pcbClient;
};

typedef std::shared_ptr<AdjustmentModuleActivateData> AdjustmentModuleActivateDataPtr;
}
#endif // ADJUSTMENTMODULECOMMON_H
