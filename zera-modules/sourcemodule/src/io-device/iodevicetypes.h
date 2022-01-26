#ifndef IODEVICETYPES_H
#define IODEVICETYPES_H

enum class IoDeviceTypes
{
    BROKEN,
    DEMO,
    ASYNCSERIAL,
};

class IoDeviceTypeQuery
{
public:
    static bool isDemo(IoDeviceTypes type) { return type == IoDeviceTypes::DEMO; }
    static bool isAsyncSerial(IoDeviceTypes type) { return type == IoDeviceTypes::ASYNCSERIAL; }
};

#endif // IODEVICETYPES_H
