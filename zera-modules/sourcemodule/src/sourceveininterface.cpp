#include "sourceveininterface.h"

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

void SourceVeinInterface::setVeinDeviceInfo(cVeinModuleActvalue *getVeinDeviceInfo)
{
    m_veinDeviceInfo = getVeinDeviceInfo;
}

void SourceVeinInterface::setVeinDeviceState(cVeinModuleActvalue *getVeinDeviceState)
{
    m_veinDeviceState = getVeinDeviceState;
}

void SourceVeinInterface::setVeinDeviceParameter(cVeinModuleParameter *getVeinDeviceParameter)
{
    m_veinDeviceParameter = getVeinDeviceParameter;
}

void SourceVeinInterface::setVeinDeviceParameterValidator(cJsonParamValidator *getVeinDeviceParameterValidator)
{
    m_veinDeviceParameterValidator = getVeinDeviceParameterValidator;
}

