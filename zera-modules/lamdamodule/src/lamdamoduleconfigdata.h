#ifndef LAMDAMODULECONFIGDATA_H
#define LAMDAMODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"

namespace LAMDAMODULE
{

struct lamdasystemconfiguration
{
    quint16 m_nInputPEntity; // entity for p input
    QString m_sInputP; // component name for p input
    quint16 m_nInputSEntity; // entity for s input
    QString m_sInputS; // component name for current input
};


class cLamdaModuleConfigData
{
public:
    cLamdaModuleConfigData(){}

    quint8 m_nDebugLevel;
    quint8 m_nLamdaSystemCount;
    int m_nModuleId;
    QList<lamdasystemconfiguration> m_lamdaSystemConfigList;
    QStringList m_lamdaChannelList;
};

}
#endif // LAMDAMODULECONFIGDATA_H
