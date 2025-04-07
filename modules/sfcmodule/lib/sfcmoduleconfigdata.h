#ifndef SFCMODULECONFIGDATA_H
#define SFCMODULECONFIGDATA_H

#include <QList>

namespace SFCMODULE
{
struct stringParameter
{
    QString m_sKey;
    QString m_sPar;
};

class cSfcModuleConfigData
{
public:
    cSfcModuleConfigData(){}
    quint8 m_nDutInpCount;
    QList<QString> m_dutInpList;
    stringParameter m_sDutInput;
};

}
#endif // SFCMODULECONFIGDATA_H
