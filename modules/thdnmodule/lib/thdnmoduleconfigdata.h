#ifndef THDNMODULECONFIGDATA_H
#define THDNMODULECONFIGDATA_H

#include <QStringList>

namespace THDNMODULE
{

// used for configuration export
struct doubleParameter
{
    QString m_sKey;
    double m_fValue;
};


class cThdnModuleConfigData
{
public:
    cThdnModuleConfigData(){}
    quint8 m_nValueCount; // how many measurment values
    QString m_sTHDType; // N or R
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    doubleParameter m_fMeasInterval; // measuring interval 0.1 .. 100.0 sec.
    double m_fmovingwindowInterval;
    bool m_bmovingWindow;
};

}
#endif // THDNMODULECONFIGDATA_H
