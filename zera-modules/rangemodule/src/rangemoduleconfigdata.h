#ifndef RANGEMODULECONFIGDATA_H
#define RANGEMODULECONFIGDATA_H

#include "socket.h"

class cRangeModuleConfigData
{
public:
    cRangeModuleConfigData(){}
    quint8 m_nDebugLevel;
    quint8 m_nChannelCount; // how many measurment channels
    QStringList m_senseChannelList; // a list of channel system names we work on
    quint8 m_nGroupCount; // the number of groups holded
    QList<int> m_GroupCountList; // the number of expected items per group
    QList<QStringList> m_GroupList; // here are our groups
    quint8 m_nGroupAct; // grouping active or not 1,0
    quint8 m_nAutoAct; // range automatic active or not 1,0
    double m_fMeasInterval; // measuring interval 0.1 .. 5.0 sec.
    double m_fAdjInterval; // adjustment interval 0.5 .. 5.0 sec.
    QStringList m_senseChannelRangeList; // the wanted ranges after configuration
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
};


#endif // RANGEMODULECONFIGDATA_H
