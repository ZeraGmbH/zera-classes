#ifndef SOURCEDEVICEBASE_H
#define SOURCEDEVICEBASE_H

#include "supportedsources.h"
#include "io-device/iodevicebrokendummy.h"
#include "io-queue/iogroupgenerator.h"
#include "io-queue/ioqueue.h"
#include "io-ids/ioidkeeper.h"

#include <QObject>

class SourceDeviceBase : public QObject
{
    Q_OBJECT
public:
    explicit SourceDeviceBase(tIoDeviceShPtr interface, SupportedSourceTypes type, QString deviceName, QString version);
    virtual ~SourceDeviceBase();

    // requests
    void setDemoDelayFollowsTimeout(bool demoDelayFollowsTimeout);

    // getter
    bool isDemo();
    QString getInterfaceDeviceInfo();

protected:
    void switchState(JsonParamApi state);
    void switchOff();
    virtual void handleSourceCmd(IoTransferDataGroup transferGroup);

    tIoDeviceShPtr m_ioInterface;
    IoGroupGenerator* m_ioGroupGenerator = nullptr;

    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;

    IoQueue m_ioQueue;
    IoIdKeeper m_currQueueId;

private slots:
    void onIoGroupFinished(IoTransferDataGroup transferGroup);

private:
    bool m_bDemoDelayFollowsTimeout = false;
};

#endif // SOURCEDEVICEBASE_H
