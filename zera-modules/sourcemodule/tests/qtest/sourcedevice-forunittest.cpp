#include "sourcedevice-forunittest.h"


SourceDeviceForUnittest::SourceDeviceForUnittest(IoDeviceBase::Ptr ioDevice, SupportedSourceTypes type, QString name, QString version) :
    SourceDevice(ioDevice, type, name, version)
{
}

void SourceDeviceForUnittest::setDemoResonseError(bool active)
{
    m_demoSimulErrorActive = active;
}

int SourceDeviceForUnittest::startTransaction(IoTransferDataGroup transferGroup)
{
    if(m_demoSimulErrorActive) {
        transferGroup.setDemoErrorOnTransfer(0);
    }
    return SourceDevice::startTransaction(transferGroup);
}
