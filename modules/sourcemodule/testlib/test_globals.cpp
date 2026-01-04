#include "test_globals.h"
#include "iodevicedemo.h"
#include "iodevicefactory.h"

IoDeviceBase::Ptr createOpenDemoIoDevice(QString deviceInfo)
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    ioDevice->open(deviceInfo);
    return ioDevice;
}

void setDemoIoFixedTimeout(IoDeviceBase::Ptr ioDevice, int timeoutMs)
{
    IoDeviceDemo *demoDev = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoDev->setResponseDelay(false, timeoutMs);
}

