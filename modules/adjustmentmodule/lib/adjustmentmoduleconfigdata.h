#ifndef ADJUSTMENTMODULECONFIGDATA_H
#define ADJUSTMENTMODULECONFIGDATA_H

#include <QString>
#include <QList>
#include <QHash>

#include "socket.h"

struct adjInfoType
{
    bool m_bAvail;
    int m_nEntity;
    QString m_sComponent;
};


class cAdjChannelInfo
{
public:
    cAdjChannelInfo(){}

    adjInfoType amplitudeAdjInfo;
    adjInfoType phaseAdjInfo;
    adjInfoType offsetAdjInfo;
    adjInfoType rangeAdjInfo;
};


class cAdjustmentModuleConfigData
{
public:
    cAdjustmentModuleConfigData(){}

    quint8 m_nDebugLevel;
    quint8 m_nAdjustmentChannelCount;
    QList<QString> m_AdjChannelList;
    adjInfoType m_ReferenceAngle;
    adjInfoType m_ReferenceFrequency;
    QHash<QString, cAdjChannelInfo*> m_AdjChannelInfoHash;
    cSocket m_RMSocket;
    cSocket m_PCBSocket;
};

#endif // ADJUSTMENTMODULECONFIGDATA_H
