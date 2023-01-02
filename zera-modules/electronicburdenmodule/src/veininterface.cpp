#include "veininterface.h"

VeinInterface::VeinInterface(QObject *parent) : QObject(parent)
{

}

VfModuleActvalue *VeinInterface::getVeinDeviceInfoComponent()
{
    return m_veinDeviceInfo;
}

VfModuleActvalue *VeinInterface::getVeinDeviceStateComponent()
{
    return m_veinDeviceState;
}

VfModuleParameter *VeinInterface::getVeinDeviceParameterComponent()
{
    return m_veinDeviceParameter;
}

cJsonParamValidator *VeinInterface::getVeinDeviceParameterValidator()
{
    return m_veinDeviceParameterValidator;
}

void VeinInterface::setVeinDeviceInfoComponent(VfModuleActvalue *veinDeviceInfo)
{
    m_veinDeviceInfo = veinDeviceInfo;
}

void VeinInterface::setVeinDeviceStateComponent(VfModuleActvalue *veinDeviceState)
{
    m_veinDeviceState = veinDeviceState;
}

void VeinInterface::setVeinDeviceParameterComponent(VfModuleParameter *veinDeviceParameter)
{
    m_veinDeviceParameter = veinDeviceParameter;
}

void VeinInterface::setVeinDeviceParameterValidator(cJsonParamValidator *veinDeviceParameterValidator)
{
    m_veinDeviceParameterValidator = veinDeviceParameterValidator;
}

