#ifndef DSPSUPERMODULECONFIGDATA_H
#define DSPSUPERMODULECONFIGDATA_H

#include <QString>

namespace DSPSUPERMODULE
{

class DspSuperModuleConfigData
{
public:
    quint32 m_maxPeriodsDsp;
    quint32 m_periodsTotal;
    quint32 m_veinUpdateMs;
};

}
#endif // DSPSUPERMODULECONFIGDATA_H
