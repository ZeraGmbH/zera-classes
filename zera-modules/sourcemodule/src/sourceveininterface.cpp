#include "sourceveininterface.h"

namespace SOURCEMODULE
{

cSourceVeinInterface::cSourceVeinInterface(QObject *parent) : QObject(parent)
{
}

cVeinModuleActvalue *cSourceVeinInterface::getVeinDeviceInfo()
{
    return m_veinDeviceInfo;
}

cVeinModuleActvalue *cSourceVeinInterface::getVeinDeviceState()
{
    return m_veinDeviceState;
}

cVeinModuleParameter *cSourceVeinInterface::getVeinDeviceParameter()
{
    return m_veinDeviceParameter;
}

cJsonParamValidator *cSourceVeinInterface::getVeinDeviceParameterValidator()
{
    return m_veinDeviceParameterValidator;
}

void cSourceVeinInterface::setVeinDeviceInfo(cVeinModuleActvalue *getVeinDeviceInfo)
{
    m_veinDeviceInfo = getVeinDeviceInfo;
}

void cSourceVeinInterface::setVeinDeviceState(cVeinModuleActvalue *getVeinDeviceState)
{
    m_veinDeviceState = getVeinDeviceState;
}

void cSourceVeinInterface::setVeinDeviceParameter(cVeinModuleParameter *getVeinDeviceParameter)
{
    m_veinDeviceParameter = getVeinDeviceParameter;
}

void cSourceVeinInterface::setVeinDeviceParameterValidator(cJsonParamValidator *getVeinDeviceParameterValidator)
{
    m_veinDeviceParameterValidator = getVeinDeviceParameterValidator;
}

} // namespace SOURCEMODULE
