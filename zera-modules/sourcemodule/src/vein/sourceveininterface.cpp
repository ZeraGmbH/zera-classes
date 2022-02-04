#include "sourceveininterface.h"
#include <veinmoduleparameter.h>

SourceVeinInterface::SourceVeinInterface(QObject *parent) : QObject(parent)
{
}

cVeinModuleActvalue *SourceVeinInterface::getVeinDeviceInfo()
{
    return m_veinDeviceInfo;
}

cVeinModuleActvalue *SourceVeinInterface::getVeinDeviceState()
{
    return m_veinDeviceState;
}

cVeinModuleParameter *SourceVeinInterface::getVeinDeviceParameter()
{
    return m_veinDeviceParameter;
}

cJsonParamValidator *SourceVeinInterface::getVeinDeviceParameterValidator()
{
    return m_veinDeviceParameterValidator;
}

void SourceVeinInterface::setVeinDeviceInfo(cVeinModuleActvalue *veinDeviceInfo)
{
    m_veinDeviceInfo = veinDeviceInfo;
}

void SourceVeinInterface::setVeinDeviceState(cVeinModuleActvalue *veinDeviceState)
{
    m_veinDeviceState = veinDeviceState;
}

void SourceVeinInterface::setVeinDeviceParameter(cVeinModuleParameter *veinDeviceParameter)
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

