#ifndef SFCMODULECONFIGDATA_H
#define SFCMODULECONFIGDATA_H

#include "configtypes.h"
#include <QList>

namespace SFCMODULE
{

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
