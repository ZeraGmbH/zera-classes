#include "sourceveininterface.h"
#include <veinmoduleparameter.h>

SourceVeinInterface::SourceVeinInterface(QObject *parent) : QObject(parent)
{
}

cVeinModuleActvalue *SourceVeinInterface::getVeinDeviceInfoComponent()
{
    return m_veinDeviceInfo;
}

cVeinModuleActvalue *SourceVeinInterface::getVeinDeviceStateComponent()
{
    return m_veinDeviceState;
}

cVeinModuleParameter *SourceVeinInterface::getVeinDeviceParameterComponent()
{
    return m_veinDeviceParameter;
}

cJsonParamValidator *SourceVeinInterface::getVeinDeviceParameterValidator()
{
    return m_veinDeviceParameterValidator;
}

void SourceVeinInterface::setVeinDeviceInfoComponent(cVeinModuleActvalue *veinDeviceInfo)
{
    m_veinDeviceInfo = veinDeviceInfo;
}

void SourceVeinInterface::setVeinDeviceStateComponent(cVeinModuleActvalue *veinDeviceState)
{
    m_veinDeviceState = veinDeviceState;
}

void SourceVeinInterface::setVeinDeviceParameterComponent(cVeinModuleParameter *veinDeviceParameter)
{
    m_veinDeviceParameter = veinDeviceParameter;
    connect(m_veinDeviceParameter, &cVeinModuleParameter::sigValueChanged,
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

