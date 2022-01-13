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

void VeinInterface::setVeinDeviceInfo(cVeinModuleActvalue *getVeinDeviceInfo)
{
    m_veinDeviceInfo = getVeinDeviceInfo;
}

void VeinInterface::setVeinDeviceState(cVeinModuleActvalue *getVeinDeviceState)
{
    m_veinDeviceState = getVeinDeviceState;
}

void VeinInterface::setVeinDeviceParameter(cVeinModuleParameter *getVeinDeviceParameter)
{
    m_veinDeviceParameter = getVeinDeviceParameter;
}

void VeinInterface::setVeinDeviceParameterValidator(cJsonParamValidator *getVeinDeviceParameterValidator)
{
    m_veinDeviceParameterValidator = getVeinDeviceParameterValidator;
}

