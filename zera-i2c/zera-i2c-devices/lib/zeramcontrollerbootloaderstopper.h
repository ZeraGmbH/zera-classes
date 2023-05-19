#ifndef ZERAMCONTROLLERBOOTLOADERSTOPPER_H
#define ZERAMCONTROLLERBOOTLOADERSTOPPER_H

#include "zeramcontrollerio.h"

class ZeraMControllerBootloaderStopper
{
public:
    ZeraMControllerBootloaderStopper(ZeraMcontrollerIoPtr i2cCtrl);
    void stopBootloader();
private:
    ZeraMcontrollerIoPtr m_i2cCtrl;
    bool m_bootloaderStopped = false;
};

#endif // ZERAMCONTROLLERBOOTLOADERSTOPPER_H
