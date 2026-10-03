#ifndef RANGEMODULECONFIGDATA_H
#define RANGEMODULECONFIGDATA_H

#include "configtypes.h"
#include <QList>

#include <networkconnectioninfo.h>

namespace RANGEMODULE
{

struct cObsermaticConfPar
{
    void setCurrentRange(int channelIdx, const QString &rangeAlias) {
        m_senseChannelRangeParameter[channelIdx].m_sValue = rangeAlias;
    }
    const QString &getCurrentRange(int channelIdx) {
        return m_senseChannelRangeParameter[channelIdx].m_sValue;
    }
    boolParameter m_nGroupAct; // grouping active or not 1,0
    boolParameter m_nRangeAutoAct; // range automatic active or not 1,0
    intParameter m_time;
private:
    friend class cRangeModuleConfiguration;
    QList<stringParameter> m_senseChannelRangeParameter;
};

struct adjustConfPar
{
    double m_fAdjInterval = 0.0; // adjustment interval 0.5 .. 5.0 sec.
    boolParameter m_ignoreRmsValuesEnable;
    doubleParameter m_ignoreRmsValuesThreshold;
    QList<boolParameter> m_senseChannelInvertParameter;
};

class cRangeModuleConfigData
{
public:
    stringParameter m_session;
    quint8 m_nChannelCount = 0;     // how many measurment channels
    QStringList m_senseChannelList; // a list of channel system names we work on
    quint8 m_nSubDCCount = 0;       // how many channels for subtract dc
    QStringList m_subdcChannelList; // a list for which channels we have to subtract dc
    quint8 m_nGroupCount = 0;       // the number of groups holded
    QList<int> m_GroupCountList;    // the number of expected items per group
    QList<QStringList> m_GroupList; // here are our groups
    cObsermaticConfPar m_ObsermaticConfPar;
    adjustConfPar m_adjustConfPar;
    double m_fMeasInterval = 0.0;   // measuring interval 0.1 .. 5.0 sec.
};

}

#endif // RANGEMODULECONFIGDATA_H
