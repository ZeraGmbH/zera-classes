#ifndef DFTMODULECONFIGDATA_H
#define DFTMODULECONFIGDATA_H

#include "configtypes.h"
#include <QList>

namespace DFTMODULE
{

class cDftModuleConfigData
{
public:
    quint8 m_nDftOrder = 0;
    quint8 m_nValueCount = 0; // how many measurment values
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    QStringList m_rfieldChannelList;
    doubleParameter m_fMeasInterval; // measuring interval 0.1 .. 100.0 sec.
    double m_fmovingwindowInterval = 0;
    stringParameter m_sRefChannel; // the reference channel's name
    bool m_bRefChannelOn = false; // if we shall take reference angle in account
    bool m_bmovingWindow = false;
};

}
#endif // DFTMODULECONFIGDATA_H
