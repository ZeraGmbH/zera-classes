#include "zeramcontrollerbootloaderstopper.h"

ZeraMControllerBootloaderStopper::ZeraMControllerBootloaderStopper(ZeraMcontrollerIoPtr i2cCtrl) :
    m_i2cCtrl(i2cCtrl)
{
}

void ZeraMControllerBootloaderStopper::stopBootloader()
{
    if(!m_bootloaderStopped) {
        m_i2cCtrl->bootloaderStartProgram();
        m_bootloaderStopped = true;
    }
}
