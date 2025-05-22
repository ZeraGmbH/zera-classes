#include "sourcedevicefacadetemplate.h"
#include "iodevicetypes.h"
#include "vfmodulecomponent.h"
#include "vfmoduleparameter.h"
#include <jsonparamvalidator.h>

IoIdGenerator SourceDeviceFacadeTemplate::m_idGenerator;

SourceDeviceFacadeTemplate::SourceDeviceFacadeTemplate(IoDeviceBase::Ptr ioDevice, ISourceIo::Ptr sourceIo) :
    m_ioDevice(ioDevice),
    m_sourceIo(sourceIo),
    m_ID(m_idGenerator.nextID())
{
    m_deviceStatusJsonApi.setDeviceInfo(m_ioDevice->getDeviceInfo());
}

int SourceDeviceFacadeTemplate::getId()
{
    return m_ID;
}

QString SourceDeviceFacadeTemplate::getIoDeviceInfo() const
{
    return m_ioDevice->getDeviceInfo();
}

bool SourceDeviceFacadeTemplate::hasDemoIo() const
{
    return m_ioDevice->getType() == IoDeviceTypes::DEMO;
}

QStringList SourceDeviceFacadeTemplate::getLastErrors() const
{
    return m_deviceStatusJsonApi.getErrors();
}

void SourceDeviceFacadeTemplate::setVeinParamStructure(QJsonObject paramStruct)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceInfoComponent()->setValue(paramStruct);
        m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(paramStruct);
    }
}

void SourceDeviceFacadeTemplate::setVeinDeviceState(QJsonObject deviceState)
{
    if(m_veinInterface)
        m_veinInterface->getVeinDeviceStateComponent()->setValue(deviceState);
}

void SourceDeviceFacadeTemplate::setVeinParamState(QJsonObject paramState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceParameterComponent()->setValue(paramState);
    }
}

void SourceDeviceFacadeTemplate::resetVeinComponents()
{
    if(m_veinInterface) {
        setVeinParamStructure(QJsonObject());
        setVeinParamState(QJsonObject());
        setVeinDeviceState(QJsonObject());
        disconnect(m_veinInterface, &SourceVeinInterface::sigNewLoadParams,
                   this, &SourceDeviceFacadeTemplate::switchLoad);
        m_veinInterface = nullptr;
    }
}

