#ifndef SOURCEDEVICESTATUSWATCHER_H
#define SOURCEDEVICESTATUSWATCHER_H

#include "sourcedeviceobserver.h"
#include "sourcedevice.h"
#include "json/jsondevicestatusapi.h"

#include <QObject>

class SourceDeviceStatusWatcherJson : public SourceDeviceObserver
{
    Q_OBJECT
public:
    SourceDeviceStatusWatcherJson(SourceDevice* sourceDevice);

signals:

protected:
    virtual void updateResponse(IoWorkerCmdPack cmdPack);

private:
    JsonDeviceStatusApi m_deviceStatus;
};

#endif // SOURCEDEVICESTATUSWATCHER_H
