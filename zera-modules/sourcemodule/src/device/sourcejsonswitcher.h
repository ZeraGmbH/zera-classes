#ifndef SOURCEJSONSWITCHER_H
#define SOURCEJSONSWITCHER_H

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
class SourceJsonSwitcher : public SourceDeviceObserver
{
    Q_OBJECT
public:
    SourceJsonSwitcher(SourceDevice* sourceDevice);
    virtual ~SourceJsonSwitcher();

    void switchState(QJsonObject state);
    void switchOff();

    QJsonObject getCurrParamState();
signals:
    void sigCurrParamTouched();
    void sigBusyChanged(bool busy);

protected:
    virtual void updateResponse(SourceWorkerCmdPack cmdPack) override;

private:
    SourceDevice* m_sourceDevice;

    PersistentJsonState* m_persistentParamState;
    SourceIoPacketGenerator* m_outInGenerator = nullptr;

    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;

signals:
    void sigBusyChangedQueued(bool busy);
};

#endif // SOURCEJSONSWITCHER_H
