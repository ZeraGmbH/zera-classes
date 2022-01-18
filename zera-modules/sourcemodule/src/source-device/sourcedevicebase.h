#ifndef SOURCEDEVICEBASE_H
#define SOURCEDEVICEBASE_H

#include "supportedsources.h"
#include "io-interface/iointerfacebase.h"
#include "io-worker/iogroupgenerator.h"
#include "io-worker/ioworker.h"
#include "io-ids/ioidkeeper.h"

#include <QObject>

class SourceDeviceBase : public QObject
{
    Q_OBJECT
public:
    explicit SourceDeviceBase(tIoInterfaceShPtr interface, SupportedSourceTypes type, QString deviceName, QString version);
    virtual ~SourceDeviceBase();

    // requests
    void setDemoDelayFollowsTimeout(bool demoDelayFollowsTimeout);

    // getter
    bool isDemo();
    QString getInterfaceDeviceInfo();

protected:
    void switchState(JsonParamApi state);
    void switchOff();
    virtual void handleSourceCmd(IoMultipleTransferGroup transferGroup);

    tIoInterfaceShPtr m_ioInterface;
    IoGroupGenerator* m_ioGroupGenerator = nullptr;

    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;

    IoWorker m_ioWorker;
    IoIdKeeper m_currWorkerId;

private slots:
    void onIoGroupFinished(IoMultipleTransferGroup transferGroup);

private:
    bool m_bDemoDelayFollowsTimeout = false;
};

#endif // SOURCEDEVICEBASE_H
