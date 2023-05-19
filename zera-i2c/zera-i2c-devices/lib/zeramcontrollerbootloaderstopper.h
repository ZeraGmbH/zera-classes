#ifndef ZERAMCONTROLLERBOOTLOADERSTOPPER_H
#define ZERAMCONTROLLERBOOTLOADERSTOPPER_H

#include "zeramcontrolleriotemplate.h"
#include <timerfactoryqt.h>

class ZERA_I2C_DEVICES_EXPORT ZeraMControllerBootloaderStopper : public QObject
{
    Q_OBJECT
public:
    ZeraMControllerBootloaderStopper(ZeraMcontrollerIoPtr i2cCtrl, int channelId);
    void stopBootloader(int msWaitForApplicationStart = 1000);
signals:
    void sigAssumeBootloaderStopped(int channelId);
private slots:
    void onAppStartWaitFinished();
private:
    ZeraMcontrollerIoPtr m_i2cCtrl;
    int m_channelId;
    TimerTemplateQtPtr m_appStartTimer;
    bool m_bootloaderStopped = false;
};

#endif // ZERAMCONTROLLERBOOTLOADERSTOPPER_H
