#include "iodevicefactory.h"
#include "iodevicebrokendummy.h"
#include "iodevicedemo.h"
#include "iodevicezerascpinet.h"
#include "iodevicezeraserial.h"

IoDeviceBase::Ptr IoDeviceFactory::createIoDevice(IoDeviceTypes type)
{
    IoDeviceBase* device = nullptr;
    switch (type) {
    default:
        device = new IoDeviceBrokenDummy(IoDeviceTypes::BROKEN);
        break;
    case IoDeviceTypes::DEMO:
        device = new IoDeviceDemo(IoDeviceTypes::DEMO);
        break;
    case IoDeviceTypes::ASYNCSERIAL:
        device = new IoDeviceZeraSerial(IoDeviceTypes::ASYNCSERIAL);
        break;
    case IoDeviceTypes::SCPI_NET:
        device = new IoDeviceZeraSCPINet;
        break;
    }
    return IoDeviceBase::Ptr(device);
}
