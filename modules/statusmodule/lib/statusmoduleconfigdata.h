#ifndef STATUSMODULECONFIGDATA_H
#define STATUSMODULECONFIGDATA_H

#include <QtGlobal>

namespace STATUSMODULE
{

class cStatusModuleConfigData
{
public:
    cStatusModuleConfigData(){}
    bool m_accumulator = false;
    quint32 m_veinUpdateMs = 0;
};

}

#endif // STATUSMODULECONFIGDATA_H
