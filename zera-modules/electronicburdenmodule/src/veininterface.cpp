#include "veininterface.h"

VeinInterface::VeinInterface(QObject *parent) : QObject(parent)
{

}

cVeinModuleActvalue *VeinInterface::getVeinDeviceInfo()
{
    return m_veinDeviceInfo;
}

cVeinModuleActvalue *VeinInterface::getVeinDeviceState()
{
    return m_veinDeviceState;
}

cVeinModuleParameter *VeinInterface::getVeinDeviceParameter()
{
    return m_veinDeviceParameter;
}

cJsonParamValidator *VeinInterface::getVeinDeviceParameterValidator()
{
    return m_veinDeviceParameterValidator;
}

void VeinInterface::setVeinDeviceInfo(cVeinModuleActvalue *veinDeviceInfo)
{
    m_veinDeviceInfo = veinDeviceInfo;
}

void VeinInterface::setVeinDeviceState(cVeinModuleActvalue *veinDeviceState)
{
    m_veinDeviceState = veinDeviceState;
}

void VeinInterface::setVeinDeviceParameter(cVeinModuleParameter *veinDeviceParameter)
{
    m_veinDeviceParameter = veinDeviceParameter;
}

void VeinInterface::setVeinDeviceParameterValidator(cJsonParamValidator *veinDeviceParameterValidator)
{
    m_veinDeviceParameterValidator = veinDeviceParameterValidator;
}

