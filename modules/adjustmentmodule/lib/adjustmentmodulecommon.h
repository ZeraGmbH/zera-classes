#ifndef ADJUSTMENTMODULECOMMON_H
#define ADJUSTMENTMODULECOMMON_H

#include "pcbserviceconnection.h"
#include <QHash>
#include <unordered_map>
#include <memory>

class AdjustChannelInfo
{
public:
    AdjustChannelInfo() = default;

    QString m_sAlias;
    QStringList m_sRangelist;
};

struct AdjustmentModuleCommon
{
    AdjustmentModuleCommon(ModuleNetworkParamsPtr networkParams) : m_pcbConnection(networkParams) {}
    PcbServiceConnection m_pcbConnection;

    std::unordered_map<QString/* m0,m1..*/, std::unique_ptr<AdjustChannelInfo>> m_adjustChannelInfoHash;
    QHash<QString/*UL1,UL2...*/, QString/*m0,m1*/> m_channelAliasHash;
};

typedef std::shared_ptr<AdjustmentModuleCommon> AdjustmentModuleCommonPtr;

#endif // ADJUSTMENTMODULECOMMON_H
