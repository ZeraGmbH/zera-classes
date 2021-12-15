#ifndef SOURCEDEVICEBASE_H
#define SOURCEDEVICEBASE_H

#include "sourceinterface.h"
#include "supportedsources.h"
#include "sourceiopacketgenerator.h"
#include "sourceioworker.h"

#include <QObject>

namespace SOURCEMODULE
{
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

protected slots:
    virtual void onSourceCmdFinished(cWorkerCommandPacket cmdPack);

protected:
    void switchState(QJsonObject state);
    void switchOff();

    tSourceInterfaceShPtr m_ioInterface;
    SourceIoPacketGenerator* m_outInGenerator = nullptr;

    SourceJsonParamApi m_paramsRequested;
    SourceJsonParamApi m_paramsCurrent;

    SourceIoWorker m_sourceIoWorker;
    int m_currentWorkerID = 0;

private:
    bool m_bDemoDelayFollowsTimeout = false;
};
}

#endif // SOURCEDEVICEBASE_H
