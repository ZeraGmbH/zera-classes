#ifndef DOSAGEMODULECONFIGDATA_H
#define DOSAGEMODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

namespace DOSAGEMODULE
{

struct dosagesystemconfiguration
{
    QString m_ComponentName;
    double m_fUpperLimit;
};

class cDosageModuleConfigData
{
public:
    cDosageModuleConfigData(){}

    quint8 m_nDosageSystemCount;
    int m_nModuleId;
    QList<dosagesystemconfiguration>m_DosageSystemConfigList;

};

}
#endif // DOSAGEMODULECONFIGDATA_H
