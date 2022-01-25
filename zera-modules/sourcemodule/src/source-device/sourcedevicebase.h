#ifndef SOURCEDEVICEBASE_H
#define SOURCEDEVICEBASE_H

#include "source-device/supportedsources.h"
#include "source-protocols/iogroupgenerator.h"
#include "io-queue/ioqueue.h"
#include "transaction-ids/idkeeper.h"

#include <QObject>

class SourceDeviceBase : public QObject
{
    Q_OBJECT
public:
    explicit SourceDeviceBase(IoDeviceBase::Ptr ioDevice, SupportedSourceTypes type, QString deviceName, QString version);
    virtual ~SourceDeviceBase();

    // getter
    bool isDemo();
    QString getIoDeviceInfo();

protected:
    void switchState(JsonParamApi state);
    void switchOff();
    virtual void handleSourceCmd(IoTransferDataGroup::Ptr transferGroup);

    IoDeviceBase::Ptr m_ioDevice;
    IoGroupGenerator* m_ioGroupGenerator = nullptr;

    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;

    IoQueue m_ioQueue;
    IdKeeperSingle m_currQueueId;

private slots:
    void onIoGroupFinished(IoTransferDataGroup::Ptr transferGroup);
};

#endif // SOURCEDEVICEBASE_H
