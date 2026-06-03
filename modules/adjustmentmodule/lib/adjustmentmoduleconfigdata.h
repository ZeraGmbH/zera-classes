#ifndef ADJUSTMENTMODULECONFIGDATA_H
#define ADJUSTMENTMODULECONFIGDATA_H

#include <QString>
#include <QList>
#include <QHash>

struct adjInfoType
{
    bool m_bAvail = false;
    int m_nEntity = 0;
    QString m_sComponent;
};

struct adjInfoTypeDc
{
    bool m_bAvail = false;
    bool m_denyOffsetAdjustment;
    int m_nEntity = 0;
    QString m_sComponent;
};

struct rangeInfoType
{
    int m_nEntity = 0;
    QString m_sComponent;
    QStringList m_rangesNotAdjustable;
};


class cAdjChannelInfo
{
public:
    adjInfoType acRmsValueInfo;
    adjInfoType acPhaseValueInfo;
    adjInfoTypeDc dcValueInfo;
    rangeInfoType rangeInfo;
};


class cAdjustmentModuleConfigData
{
public:
    quint8 m_nAdjustmentChannelCount = 0;
    QList<QString> m_AdjChannelList;
    adjInfoType m_ReferenceAngle;
    adjInfoType m_ReferenceFrequency;
    QHash<QString, cAdjChannelInfo*> m_AdjChannelInfoHash;
};

#endif // ADJUSTMENTMODULECONFIGDATA_H
