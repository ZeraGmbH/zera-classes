#ifndef LAMBDAMODULECONFIGDATA_H
#define LAMBDAMODULECONFIGDATA_H

#include <QString>
#include <QStringList>

namespace LAMBDAMODULE
{

struct lambdasystemconfiguration
{
    quint16 m_nInputPEntity = -1; // entity for p input
    QString m_sInputP;            // component name for p input
    quint16 m_nInputQEntity = -1; // entity for q input
    QString m_sInputQ;            // component name for q input
    quint16 m_nInputSEntity = -1; // entity for s input
    QString m_sInputS;            // component name for current input
};


class cLambdaModuleConfigData
{
public:
    cLambdaModuleConfigData(){}

    quint8 m_nLambdaSystemCount = 0;
    int m_nModuleId = 0;
    QList<lambdasystemconfiguration> m_lambdaSystemConfigList;
    QStringList m_lambdaChannelList;
    bool m_activeMeasModeAvail = false;
    quint16 m_activeMeasModeEntity = 0;
    QString m_activeMeasModeComponent;
    QString m_activeMeasModePhaseComponent;
};

}
#endif // LAMBDAMODULECONFIGDATA_H
