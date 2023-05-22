#ifndef ZERAMCONTROLLERBOOTLOADERSTOPPERFACTORY_H
#define ZERAMCONTROLLERBOOTLOADERSTOPPERFACTORY_H

#include "zeramcontrollerbootloaderstopper.h"

class ZeraMControllerBootloaderStopperFactory
{
public:
    static ZeraMControllerBootloaderStopperPtr createBootloaderStopper(ZeraMcontrollerIoPtr i2cCtrl, int channelId);
protected:
    static std::function<ZeraMControllerBootloaderStopperPtr(ZeraMcontrollerIoPtr i2cCtrl, int channelId)> m_createFunction;
};

#endif // ZERAMCONTROLLERBOOTLOADERSTOPPERFACTORY_H
