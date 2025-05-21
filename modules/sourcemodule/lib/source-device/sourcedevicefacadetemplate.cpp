#include "sourcedevicefacadetemplate.h"
#include "iodevicetypes.h"

IoIdGenerator SourceDeviceFacadeTemplate::m_idGenerator;

SourceDeviceFacadeTemplate::SourceDeviceFacadeTemplate(IoDeviceBase::Ptr ioDevice) :
    m_ioDevice(ioDevice),
    m_ID(m_idGenerator.nextID())
{
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

