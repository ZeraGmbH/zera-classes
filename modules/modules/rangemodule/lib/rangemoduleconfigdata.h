#ifndef RANGEMODULECONFIGDATA_H
#define RANGEMODULECONFIGDATA_H

#include <QString>
#include <QList>

#include <socket.h>

namespace RANGEMODULE
{

// used for configuration export

struct boolParameter
{
    QString m_sKey;
    quint8 m_nActive; // active or not 1,0
};


struct stringParameter
{
    QString m_sKey;
    QString m_sPar;
};

struct cObsermaticConfPar
{
    boolParameter m_nGroupAct; // grouping active or not 1,0
    boolParameter m_nRangeAutoAct; // range automatic active or not 1,0
    QList<stringParameter> m_senseChannelRangeParameter;
};

class cRangeModuleConfigData
{
public:
    cRangeModuleConfigData(){}
    quint8 m_nDebugLevel;
    quint8 m_nChannelCount; // how many measurment channels
    QStringList m_senseChannelList; // a list of channel system names we work on
    quint8 m_nSubDCCount; // how many channels for subtract dc
    QStringList m_subdcChannelList; // a list for which channels we have to subtract dc
    quint8 m_nGroupCount; // the number of groups holded
    QList<int> m_GroupCountList; // the number of expected items per group
    QList<QStringList> m_GroupList; // here are our groups
    cObsermaticConfPar m_ObsermaticConfPar;
    double m_fMeasInterval; // measuring interval 0.1 .. 5.0 sec.
    double m_fAdjInterval; // adjustment interval 0.5 .. 5.0 sec.
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
};

}

#endif // RANGEMODULECONFIGDATA_H
