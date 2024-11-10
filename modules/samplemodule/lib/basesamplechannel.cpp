#include "basesamplechannel.h"

cBaseSampleChannel::cBaseSampleChannel(QString name, quint8 chnnr, QString moduleChannelName) :
    cModuleActivist(moduleChannelName),
    m_sName(name),
    m_nChannelNr(chnnr)
{
    m_sUnit = "";
}

QString cBaseSampleChannel::getName()
{
    return m_sName;
}

QString cBaseSampleChannel::getAlias()
{
    return m_sAlias;
}

