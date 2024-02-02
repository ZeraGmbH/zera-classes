#ifndef LAMBDAMODULECONFIGDATA_H
#define LAMBDAMODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"

namespace LAMBDAMODULE
{

struct lambdasystemconfiguration
{
    quint16 m_nInputPEntity; // entity for p input
    QString m_sInputP; // component name for p input
    quint16 m_nInputQEntity; // entity for q input
    QString m_sInputQ; // component name for q input
    quint16 m_nInputSEntity; // entity for s input
    QString m_sInputS; // component name for current input
};


class cLambdaModuleConfigData
{
public:
    cLambdaModuleConfigData(){}

    quint8 m_nDebugLevel;
    quint8 m_nLambdaSystemCount;
    int m_nModuleId;
    QList<lambdasystemconfiguration> m_lambdaSystemConfigList;
    QStringList m_lambdaChannelList;
    bool m_activeMeasModeAvail;
    quint16 m_activeMeasModeEntity;
    QString m_activeMeasModeComponent;
    QString m_activeMeasModePhaseComponent;
};

}
#endif // LAMBDAMODULECONFIGDATA_H
