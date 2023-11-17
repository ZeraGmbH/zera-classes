#ifndef BLEMODULECONFIGDATA_H
#define BLEMODULECONFIGDATA_H

#include <QString>

namespace BLEMODULE
{

struct boolParameter
{
    QString m_sKey;
    quint8 m_nActive; // active or not 1,0
};

struct stringParameter
{
    QString m_sKey;
    QString m_sPar;
};

class cBleModuleConfigData
{
public:
    cBleModuleConfigData(){}
    boolParameter m_bluetoothOn;
    stringParameter m_macAddress;
};

}
#endif // BLEMODULECONFIGDATA_H
