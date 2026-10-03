#ifndef THDNMODULECONFIGDATA_H
#define THDNMODULECONFIGDATA_H

#include "configtypes.h"

namespace THDNMODULE
{

class cThdnModuleConfigData
{
public:
    cThdnModuleConfigData(){}
    int m_thdrSourceEntity = 0;
    doubleParameter m_fMeasInterval; // measuring interval 0.1 .. 100.0 sec.
    double m_fmovingwindowInterval = 0.0;
    bool m_bmovingWindow = false;
};

}
#endif // THDNMODULECONFIGDATA_H
