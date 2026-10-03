#ifndef REFERENCEMODULECONFIGDATA_H
#define REFERENCEMODULECONFIGDATA_H

#include <QStringList>

namespace REFERENCEMODULE
{

class cReferenceModuleConfigData
{
public:
    cReferenceModuleConfigData(){}
    quint8 m_nChannelCount = 0;         // how many measurement channels
    QStringList m_referenceChannelList; // a list of channel system names we work on

    double m_fMeasInterval = 0.0;       // measuring interval 1.0 .. 10.0 sec.
    int m_nIgnore = 0;                  // how many measurement values we ignore before adjustment start
};

}

#endif // REFERENCEMODULECONFIGDATA_H
