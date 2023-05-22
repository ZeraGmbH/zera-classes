#include "zeramcontrollerbootloaderstopperfactory.h"

std::function<ZeraMControllerBootloaderStopperPtr(ZeraMcontrollerIoPtr i2cCtrl, int channelId)> ZeraMControllerBootloaderStopperFactory::m_createFunction =
        [](ZeraMcontrollerIoPtr i2cCtrl, int channelId)
{
    return std::make_shared<ZeraMControllerBootloaderStopper>(i2cCtrl, channelId);
};

ZeraMControllerBootloaderStopperPtr ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(ZeraMcontrollerIoPtr i2cCtrl, int channelId)
{
    return m_createFunction(i2cCtrl, channelId);
}
