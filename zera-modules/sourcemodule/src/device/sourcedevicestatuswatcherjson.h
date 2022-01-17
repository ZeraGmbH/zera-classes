#ifndef SOURCEDEVICESTATUSWATCHER_H
#define SOURCEDEVICESTATUSWATCHER_H

#include "sourcedeviceobserver.h"
#include "sourcedevice.h"
#include "json/jsondevicestatusapi.h"

#include <QObject>
#include <QTimer>

/*
 * Periodic actor / observer on source device
 * Poll status
 * notify current state changed
 */

enum class SOURCE_STATE
{
    UNDEFINED,
    IDLE,
    SWITCH_BUSY,
    ERROR_SWITCH,
    ERROR_POLL
};

class SourceDeviceStatusWatcherJson : public SourceDeviceObserver
{
    Q_OBJECT
public:
    SourceDeviceStatusWatcherJson(SourceDevice* sourceDevice);

    void setPollTime(int ms);

signals:
    void sigStateChanged(SOURCE_STATE state);

protected:
    virtual void updateResponse(IoWorkerCmdPack cmdPack);

private slots:
    void onPollTimer();
    void onSwitchBusyChanged();
private:
    bool isErrorActive();
    void enablePolling();
    void disablePolling();
    void setState(SOURCE_STATE state);
    void setPollingOnStateChange();
    void handleSwitchResponse(IoWorkerCmdPack &cmdPack);
    void handleStateResponse(IoWorkerCmdPack &cmdPack);

    SourceDevice* m_sourceDevice;
    SOURCE_STATE m_stateEnum;
    IoIdKeeper m_ioIdKeeper;

    QTimer m_pollTimer;
};

#endif // SOURCEDEVICESTATUSWATCHER_H
