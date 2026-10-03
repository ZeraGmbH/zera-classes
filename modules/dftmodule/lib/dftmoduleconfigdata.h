#ifndef DFTMODULECONFIGDATA_H
#define DFTMODULECONFIGDATA_H

#include "configtypes.h"
#include <QList>

namespace DFTMODULE
{

class cDftModuleConfigData
{
public:
    cDftModuleConfigData(){}
    quint8 m_nDftOrder;
    quint8 m_nValueCount; // how many measurment values
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    QStringList m_rfieldChannelList;
    doubleParameter m_fMeasInterval; // measuring interval 0.1 .. 100.0 sec.
    double m_fmovingwindowInterval;
    stringParameter m_sRefChannel; // the reference channel's name
    bool m_bRefChannelOn; // if we shall take reference angle in account
    bool m_bmovingWindow;
};

}
#endif // DFTMODULECONFIGDATA_H
