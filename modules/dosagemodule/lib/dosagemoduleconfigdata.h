#ifndef DOSAGEMODULECONFIGDATA_H
#define DOSAGEMODULECONFIGDATA_H

#include <QString>
#include <QList>

namespace DOSAGEMODULE
{

struct dosagesystemconfiguration
{
    int m_nEntity = 0;
    QString m_ComponentName;
    double m_fUpperLimit = 0.0;
};

class cDosageModuleConfigData
{
public:
    quint8 m_nDosageSystemCount = 0;
    int m_nModuleId = 0;
    QList<dosagesystemconfiguration>m_DosageSystemConfigList;

};

}
#endif // DOSAGEMODULECONFIGDATA_H
