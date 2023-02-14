#ifndef ADJUSTMENTMODULECOMMON_H
#define ADJUSTMENTMODULECOMMON_H

#include <pcbinterface.h>
#include <rminterface.h>
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
    Zera::PcbInterfacePtr m_pcbInterface;
    Zera::ProxyClientPtr m_pcbClient;
    Zera::RMInterfacePtr m_rmInterface;
    Zera::ProxyClientPtr m_rmClient;

    std::unordered_map<QString/* m0,m1..*/, std::unique_ptr<AdjustChannelInfo>> m_adjustChannelInfoHash;
    QHash<QString/*UL1,UL2...*/, QString/*m0,m1*/> m_channelAliasHash;
};

const int CONNECTION_TIMEOUT = 25000;
const int TRANSACTION_TIMEOUT = 3000;

typedef std::shared_ptr<AdjustmentModuleCommon> AdjustmentModuleCommonPtr;

#endif // ADJUSTMENTMODULECOMMON_H
