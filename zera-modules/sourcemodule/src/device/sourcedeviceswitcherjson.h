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
 * keep switch state persistent
 * notify switch finished
 */
class SourceDeviceSwitcherJson : public SourceDeviceObserver
{
    Q_OBJECT
public:
    SourceDeviceSwitcherJson(SourceDevice* sourceDevice);
    virtual ~SourceDeviceSwitcherJson();

    void switchState(JsonParamApi paramState);
    void switchOff();

    JsonParamApi getCurrLoadState();
signals:
    void sigSwitchFinished();

protected:
    virtual void updateResponse(IoWorkerCmdPack cmdPack) override;

private:
    SourceDevice* m_sourceDevice;

    PersistentJsonState* m_persistentParamState;

    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;
};

#endif // SOURCEDEVICESWITCHERJSON_H
