#ifndef SOURCEDEVICEBASE_H
#define SOURCEDEVICEBASE_H

#include "sourceinterface.h"
#include "supportedsources.h"
#include "sourceiopacketgenerator.h"
#include "sourceioworker.h"
#include "sourceidkeeper.h"

#include <QObject>

class SourceDeviceBase : public QObject
{
    Q_OBJECT
public:
    explicit SourceDeviceBase(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString deviceName, QString version);
    virtual ~SourceDeviceBase();

    // requests
    void setDemoDelayFollowsTimeout(bool demoDelayFollowsTimeout);

    // getter
    bool isDemo();
    QString getInterfaceDeviceInfo();

protected:
    void switchState(QJsonObject state);
    void switchOff();
    virtual void handleSourceCmd(SourceWorkerCmdPack cmdPack);

    tSourceInterfaceShPtr m_ioInterface;
    SourceIoPacketGenerator* m_outInGenerator = nullptr;

    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;

    SourceIoWorker m_sourceIoWorker;
    SourceIdKeeper m_currWorkerId;

private slots:
    void onSourceCmdFinished(SourceWorkerCmdPack cmdPack);

private:
    bool m_bDemoDelayFollowsTimeout = false;
};

#endif // SOURCEDEVICEBASE_H
