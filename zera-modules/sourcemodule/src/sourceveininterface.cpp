#include "sourceveininterface.h"

namespace SOURCEMODULE
{

cSourceVeinInterface::cSourceVeinInterface(QObject *parent) : QObject(parent)
{
}

cVeinModuleActvalue *cSourceVeinInterface::veinDeviceInfo()
{
    return m_veinDeviceInfo;
}

cVeinModuleActvalue *cSourceVeinInterface::veinDeviceState()
{
    return m_veinDeviceState;
}

cVeinModuleParameter *cSourceVeinInterface::veinDeviceParameter()
{
    return m_veinDeviceParameter;
}

cJsonParamValidator *cSourceVeinInterface::veinDeviceParameterValidator()
{
    return m_veinDeviceParameterValidator;
}

void cSourceVeinInterface::setVeinDeviceInfo(cVeinModuleActvalue *veinDeviceInfo)
{
    m_veinDeviceInfo = veinDeviceInfo;
}

void cSourceVeinInterface::setVeinDeviceState(cVeinModuleActvalue *veinDeviceState)
{
    m_veinDeviceState = veinDeviceState;
}

void cSourceVeinInterface::setVeinDeviceParameter(cVeinModuleParameter *veinDeviceParameter)
{
    m_veinDeviceParameter = veinDeviceParameter;
}

void cSourceVeinInterface::setVeinDeviceParameterValidator(cJsonParamValidator *veinDeviceParameterValidator)
{
    m_veinDeviceParameterValidator = veinDeviceParameterValidator;
}

} // namespace SOURCEMODULE
