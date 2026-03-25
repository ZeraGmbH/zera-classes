#ifndef THDNMODULECONFIGDATA_H
#define THDNMODULECONFIGDATA_H

#include <QStringList>

namespace THDNMODULE
{

// used for configuration export
struct doubleParameter
{
    QString m_sKey;
    double m_fValue = 0.0;
};

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
