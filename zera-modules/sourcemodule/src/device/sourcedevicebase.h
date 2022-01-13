#ifndef SOURCEDEVICEBASE_H
#define SOURCEDEVICEBASE_H

#include "supportedsources.h"
#include "io-interface/iointerfacebase.h"
#include "io-interface/iopacketgenerator.h"
#include "io-interface/ioworker.h"
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
    virtual void handleSourceCmd(IoWorkerCmdPack cmdPack);

    tIoInterfaceShPtr m_ioInterface;
    IoPacketGenerator* m_outInGenerator = nullptr;

    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;

    IoWorker m_sourceIoWorker;
    IoIdKeeper m_currWorkerId;

private slots:
    void onSourceCmdFinished(IoWorkerCmdPack cmdPack);

private:
    bool m_bDemoDelayFollowsTimeout = false;
};

#endif // SOURCEDEVICEBASE_H
