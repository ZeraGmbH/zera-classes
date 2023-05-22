#ifndef ZERAMCONTROLLERBOOTLOADERSTOPPERINTERFACE_H
#define ZERAMCONTROLLERBOOTLOADERSTOPPERINTERFACE_H

#include "zera-i2c-devices_export.h"
#include <QObject>
#include <memory>

class ZERA_I2C_DEVICES_EXPORT ZeraMControllerBootloaderStopperInterface : public QObject
{
    Q_OBJECT
public:
    virtual void stopBootloader(int msWaitForApplicationStart = 1000) = 0;
signals:
    void sigAssumeBootloaderStopped(int channelId);
};

typedef std::shared_ptr<ZeraMControllerBootloaderStopperInterface> ZeraMControllerBootloaderStopperPtr;

#endif // ZERAMCONTROLLERBOOTLOADERSTOPPERINTERFACE_H
