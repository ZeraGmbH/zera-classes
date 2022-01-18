#include "iodevicefactory.h"
#include "iodevicebaseserial.h"
#include "iodevicedemo.h"
#include "iodevicezeraserial.h"

tIoDeviceShPtr IoDeviceFactory::createIoDevice(IoDeviceTypes type)
{
    IODeviceBaseSerial* device = nullptr;
    switch (type) {
    case SERIAL_DEVICE_BASE:
        device = new IODeviceBaseSerial();
        break;
    case SERIAL_DEVICE_DEMO:
        device = new IoDeviceDemo();
        break;
    case SERIAL_DEVICE_ASYNCSERIAL:
        device = new IoDeviceZeraSerial();
        break;
    case SERIAL_DEVICE_TYPE_COUNT:
        qCritical("Do not use SERIAL_DEVICE_TYPE_COUNT");
        break;
    }
    return tIoDeviceShPtr(device);
}
