#ifndef TRANSFORMER1MODULECONFIGDATA_H
#define TRANSFORMER1MODULECONFIGDATA_H

#include "configtypes.h"
#include <QStringList>

namespace TRANSFORMER1MODULE
{

struct transformersystemconfiguration
{
    QString m_sInputPrimaryVector; // component name for primary vector
    QString m_sInputSecondaryVector; // component name for secondary vector
};

class cTransformer1ModuleConfigData
{
public:
    quint8 m_nTransformerSystemCount = 0;
    int m_nModuleId = 0;
    QList<transformersystemconfiguration> m_transformerSystemConfigList;
    QString m_clampUnit; // 4 chars defining unit of primclampprim, primclampsec, secclamprim, secclampsec, primdut, secdut
    QStringList m_TransformerChannelList;
    doubleParameter primClampPrim;
    doubleParameter primClampSec;
    doubleParameter secClampPrim;
    doubleParameter secClampSec;
    doubleParameter dutPrim;
    doubleParameter dutSec;
};

}
#endif // TRANSFORMER1MODULECONFIGDATA_H
