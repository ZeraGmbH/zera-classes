#ifndef SOURCEDEVICESWITCHERJSON_H
#define SOURCEDEVICESWITCHERJSON_H

#include "sourcedeviceobserver.h"
#include "sourcedevice.h"

#include <QObject>
#include <QJsonObject>

class PersistentJsonState;

/*
 * Actor/Observer on source device
 * Switch on/off
 * notify busy changed
 * notify current state changed
 */
class SourceDeviceSwitcherJson : public SourceDeviceObserver
{
    Q_OBJECT
public:
    SourceDeviceSwitcherJson(SourceDevice* sourceDevice);
    virtual ~SourceDeviceSwitcherJson();

    void switchState(QJsonObject state);
    void switchOff();

    QJsonObject getCurrParamState();
signals:
    void sigCurrParamTouched();
    void sigBusyChanged(bool busy);

protected:
    virtual void updateResponse(IoWorkerCmdPack cmdPack) override;

private:
    SourceDevice* m_sourceDevice;

    PersistentJsonState* m_persistentParamState;
    IoPacketGenerator* m_outInGenerator = nullptr;

    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;

signals:
    void sigBusyChangedQueued(bool busy);
};

#endif // SOURCEDEVICESWITCHERJSON_H
