#ifndef IODEVICEFACTORY_H
#define IODEVICEFACTORY_H

#include <QSharedPointer>

enum IoDeviceTypes
{
    SERIAL_DEVICE_BASE = 0,
    SERIAL_DEVICE_DEMO,
    SERIAL_DEVICE_ASYNCSERIAL,

    SERIAL_DEVICE_TYPE_COUNT
};

class IODeviceBaseSerial;
typedef QSharedPointer<IODeviceBaseSerial> tIoDeviceShPtr;

// create interfaces through IoDeviceFactory only
class IoDeviceFactory {
public:
    static tIoDeviceShPtr createIoDevice(IoDeviceTypes type);
};

#endif // IODEVICEFACTORY_H
