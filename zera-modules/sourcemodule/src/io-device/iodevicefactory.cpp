#include "iodevicefactory.h"
#include "iodevicebrokendummy.h"
#include "iodevicedemo.h"
#include "iodevicezeraserial.h"

IoDeviceBase::Ptr IoDeviceFactory::createIoDevice(IoDeviceTypes type)
{
    IoDeviceBase* device = nullptr;
    switch (type) {
    case SERIAL_DEVICE_BROKEN:
        device = new IoDeviceBrokenDummy(SERIAL_DEVICE_BROKEN);
        break;
    case SERIAL_DEVICE_DEMO:
        device = new IoDeviceDemo(SERIAL_DEVICE_DEMO);
        break;
    case SERIAL_DEVICE_ASYNCSERIAL:
        device = new IoDeviceZeraSerial(SERIAL_DEVICE_ASYNCSERIAL);
        break;
    case SERIAL_DEVICE_TYPE_COUNT:
        qCritical("Do not use SERIAL_DEVICE_TYPE_COUNT");
        break;
    }
    return IoDeviceBase::Ptr(device);
}
