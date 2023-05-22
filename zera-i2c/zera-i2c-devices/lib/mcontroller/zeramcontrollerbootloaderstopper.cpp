#include "zeramcontrollerbootloaderstopper.h"

ZeraMControllerBootloaderStopper::ZeraMControllerBootloaderStopper(ZeraMcontrollerIoPtr i2cCtrl, int channelId) :
    m_i2cCtrl(i2cCtrl),
    m_channelId(channelId)
{
}

void ZeraMControllerBootloaderStopper::stopBootloader(int msWaitForApplicationStart)
{
    if(!m_bootloaderStopped && !m_appStartTimer) {
        ZeraMControllerIoTemplate::atmelRM result = m_i2cCtrl->bootloaderStartProgram();
        if(result == ZeraMControllerIoTemplate::cmddone) {
            qInfo("Stopping bootloader succeeded - wait %ims for app startup...", msWaitForApplicationStart);
            m_appStartTimer = TimerFactoryQt::createSingleShot(msWaitForApplicationStart);
            connect(m_appStartTimer.get(), &TimerTemplateQt::sigExpired,
                    this, &ZeraMControllerBootloaderStopper::onAppStartWaitFinished);
            m_appStartTimer->start();
        }
        else {
            qInfo("Stopping bootloader failed. Either application running or no controller connected.");
            m_bootloaderStopped = true;
            emit sigAssumeBootloaderStopped(m_channelId);
        }
    }
}

void ZeraMControllerBootloaderStopper::onAppStartWaitFinished()
{
    qInfo("Waiting for µC-application start finished.");
    m_bootloaderStopped = true;
    emit sigAssumeBootloaderStopped(m_channelId);
}
