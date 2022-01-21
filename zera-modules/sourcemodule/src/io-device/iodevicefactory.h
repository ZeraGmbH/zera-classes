#ifndef IODEVICEFACTORY_H
#define IODEVICEFACTORY_H

#include "iodevicebase.h"
#include "iodevicetypes.h"

// create interfaces through IoDeviceFactory only
class IoDeviceFactory {
public:
    static IoDeviceBase::Ptr createIoDevice(IoDeviceTypes type);
};

#endif // IODEVICEFACTORY_H
