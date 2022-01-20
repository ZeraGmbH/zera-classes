#include "sourcedevice-forunittest.h"


SourceDeviceForUnittest::SourceDeviceForUnittest(tIoDeviceShPtr interface, SupportedSourceTypes type, QString name, QString version) :
    SourceDevice(interface, type, name, version)
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
