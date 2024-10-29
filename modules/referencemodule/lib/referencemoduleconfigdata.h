#ifndef REFERENCEMODULECONFIGDATA_H
#define REFERENCEMODULECONFIGDATA_H

#include <QString>
#include <QList>

#include "networkconnectioninfo.h"

namespace REFERENCEMODULE
{

class cReferenceModuleConfigData
{
public:
    cReferenceModuleConfigData(){}
    quint8 m_nChannelCount; // how many measurement channels
    QStringList m_referenceChannelList; // a list of channel system names we work on

    double m_fMeasInterval; // measuring interval 1.0 .. 10.0 sec.
    int m_nIgnore; // how many measurement values we ignore before adjustment start
};

}

#endif // REFERENCEMODULECONFIGDATA_H
