#ifndef TRANSFORMER1MODULECONFIGDATA_H
#define TRANSFORMER1MODULECONFIGDATA_H

#include <QString>
#include <QStringList>
#include <QList>

#include "socket.h"
#include "foutinfo.h"

namespace TRANSFORMER1MODULE
{

struct transformersystemconfiguration
{
    QString m_sInputPrimaryVector; // component name for primary vector
    QString m_sInputSecondaryVector; // component name for secondary vector
};


// used for configuration export
struct doubleParameter
{
    QString m_sKey;
    double m_fValue;
};


class cTransformer1ModuleConfigData
{
public:
    cTransformer1ModuleConfigData(){}

    quint8 m_nDebugLevel;
    quint8 m_nTransformerSystemCount;
    int m_nModuleId;
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
