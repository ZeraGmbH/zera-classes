#include "sourceveininterface.h"
#include <vfmoduleparameter.h>

SourceVeinInterface::SourceVeinInterface()
{
}

VfModuleComponent *SourceVeinInterface::getVeinDeviceInfoComponent()
{
    return m_veinDeviceInfo;
}

VfModuleComponent *SourceVeinInterface::getVeinDeviceStateComponent()
{
    return m_veinDeviceState;
}

VfModuleParameter *SourceVeinInterface::getVeinDeviceParameterComponent()
{
    return m_veinDeviceParameter;
}

cJsonParamValidator *SourceVeinInterface::getVeinDeviceParameterValidator()
{
    return m_veinDeviceParameterValidator;
}

void SourceVeinInterface::setVeinDeviceInfoComponent(VfModuleComponent *veinDeviceInfo)
{
    m_veinDeviceInfo = veinDeviceInfo;
}

void SourceVeinInterface::setVeinDeviceStateComponent(VfModuleComponent *veinDeviceState)
{
    m_veinDeviceState = veinDeviceState;
}

void SourceVeinInterface::setVeinDeviceParameterComponent(VfModuleParameter *veinDeviceParameter)
{
    m_veinDeviceParameter = veinDeviceParameter;
    connect(m_veinDeviceParameter, &VfModuleParameter::sigValueChanged,
            this, &SourceVeinInterface::onNewVeinLoadParams);
}

void SourceVeinInterface::setVeinDeviceParameterValidator(cJsonParamValidator *veinDeviceParameterValidator)
{
    m_veinDeviceParameterValidator = veinDeviceParameterValidator;
}

void SourceVeinInterface::onNewVeinLoadParams(QVariant params)
{
    emit sigNewLoadParams(params.toJsonObject());
}

