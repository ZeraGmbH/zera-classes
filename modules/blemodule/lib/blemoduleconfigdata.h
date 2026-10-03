#ifndef BLEMODULECONFIGDATA_H
#define BLEMODULECONFIGDATA_H

#include "configtypes.h"

namespace BLEMODULE
{

class cBleModuleConfigData
{
public:
    boolParameter m_bluetoothOn;
    stringParameter m_macAddress;
};

}
#endif // BLEMODULECONFIGDATA_H
