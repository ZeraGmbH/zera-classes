#ifndef ADJUSTMENTMODULECOMMON_H
#define ADJUSTMENTMODULECOMMON_H

#include "pcbinterface.h"
#include "rminterface.h"
#include "proxy.h"
#include <QHash>
#include <unordered_map>
#include <memory>

class cAdjustChannelInfo
{
public:
    cAdjustChannelInfo(){}

    QString m_sAlias;
    QStringList m_sRangelist;
};

struct AdjustmentModuleCommon
{
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    Zera::Proxy::ProxyClientPtr m_pcbClient;
    Zera::Server::RMInterfacePtr m_rmInterface;
    Zera::Proxy::ProxyClientPtr m_rmClient;

    std::unordered_map<QString, std::unique_ptr<cAdjustChannelInfo>> m_adjustChannelInfoHash;
    QHash<QString, QString> m_AliasChannelHash; // we use this hash for easier access
};

typedef std::shared_ptr<AdjustmentModuleCommon> AdjustmentModuleCommonPtr;

#endif // ADJUSTMENTMODULECOMMON_H
