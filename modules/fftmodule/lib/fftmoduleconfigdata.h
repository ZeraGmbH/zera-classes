#ifndef FFTMODULECONFIGDATA_H
#define FFTMODULECONFIGDATA_H

#include <QString>
#include <QList>

namespace FFTMODULE
{

// used for configuration export
struct doubleParameter
{
    QString m_sKey;
    double m_fValue;
};


struct stringParameter
{
    QString m_sKey;
    QString m_sPar;
};


class cFftModuleConfigData
{
public:
    cFftModuleConfigData(){}
    quint8 m_nFftOrder;
    quint8 m_nValueCount; // how many measurement values
    QStringList m_valueChannelList; // a list of channel or channel pairs we work on to generate our measurement values
    stringParameter m_RefChannel;
    doubleParameter m_fMeasInterval; // measuring interval 0.1 .. 100.0 sec.
    double m_fmovingwindowInterval;
    bool m_bmovingWindow;
};

}
#endif // FFTMODULECONFIGDATA_H
