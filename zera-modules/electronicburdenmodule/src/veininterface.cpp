#include "veininterface.h"

VeinInterface::VeinInterface(QObject *parent) : QObject(parent)
{

}

cVeinModuleActvalue *VeinInterface::getVeinDeviceInfoComponent()
{
    return m_veinDeviceInfo;
}

cVeinModuleActvalue *VeinInterface::getVeinDeviceStateComponent()
{
    return m_veinDeviceState;
}

cVeinModuleParameter *VeinInterface::getVeinDeviceParameterComponent()
{
    return m_veinDeviceParameter;
}

cJsonParamValidator *VeinInterface::getVeinDeviceParameterValidator()
{
    return m_veinDeviceParameterValidator;
}

void VeinInterface::setVeinDeviceInfoComponent(cVeinModuleActvalue *veinDeviceInfo)
{
    m_veinDeviceInfo = veinDeviceInfo;
}

void VeinInterface::setVeinDeviceStateComponent(cVeinModuleActvalue *veinDeviceState)
{
    m_veinDeviceState = veinDeviceState;
}

void VeinInterface::setVeinDeviceParameterComponent(cVeinModuleParameter *veinDeviceParameter)
{
    m_veinDeviceParameter = veinDeviceParameter;
}

void VeinInterface::setVeinDeviceParameterValidator(cJsonParamValidator *veinDeviceParameterValidator)
{
    m_veinDeviceParameterValidator = veinDeviceParameterValidator;
}

