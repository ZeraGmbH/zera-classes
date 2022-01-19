#include "iodevicefactory.h"
#include "iodevicebaseserial.h"
#include "iodevicedemo.h"
#include "iodevicezeraserial.h"

tIoDeviceShPtr IoDeviceFactory::createIoDevice(IoDeviceTypes type)
{
    IODeviceBaseSerial* device = nullptr;
    switch (type) {
    case SERIAL_DEVICE_BASE:
        device = new IODeviceBaseSerial(SERIAL_DEVICE_BASE);
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
    return tIoDeviceShPtr(device);
}
