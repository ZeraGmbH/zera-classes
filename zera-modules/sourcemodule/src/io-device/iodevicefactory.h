#ifndef IODEVICEFACTORY_H
#define IODEVICEFACTORY_H

#include <QSharedPointer>

enum IoDeviceTypes
{
    SERIAL_DEVICE_BROKEN,
    SERIAL_DEVICE_DEMO,
    SERIAL_DEVICE_ASYNCSERIAL,

    SERIAL_DEVICE_TYPE_COUNT
};

class IoDeviceBase;
typedef QSharedPointer<IoDeviceBase> tIoDeviceShPtr;

// create interfaces through IoDeviceFactory only
class IoDeviceFactory {
public:
    static tIoDeviceShPtr createIoDevice(IoDeviceTypes type);
};

#endif // IODEVICEFACTORY_H
