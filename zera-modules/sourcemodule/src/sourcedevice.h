#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QUuid>
#include <QStringList>
#include "sourceinterface.h"
#include "sourcedevicestatus.h"
#include "sourceiopacketgenerator.h"
#include "supportedsources.h"
#include "sourceioworker.h"

namespace SOURCEMODULE
{
class cSourceVeinInterface;

class cSourceDevice : public QObject
{
    Q_OBJECT
public:
    explicit cSourceDevice(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString version);
    virtual ~cSourceDevice();

    // requests
    bool close(QUuid uuid);
    void switchLoad(QJsonObject jsonParamsState);
    QStringList getLastErrors();
    void setDemoDelayFollowsTimeout(bool demoDelayFollowsTimeout);

    // getter
    bool isDemo();
    QString getInterfaceDeviceInfo();

    // setter
    void setVeinInterface(cSourceVeinInterface* veinInterface);
    void saveState(); // persistency

signals:
    void sigClosed(cSourceDevice* sourceDevice, QUuid uuid);

public slots:
    void onNewVeinParamStatus(QVariant paramState);
    void onSourceCmdFinished(cWorkerCommandPacket cmdPack);

private slots:
    void onInterfaceClosed();

private:
    void switchState(QJsonObject state);
    void switchOff();
    void doFinalCloseActivities();
    void setVeinParamStructure(QJsonObject paramStruct);
    void setVeinParamState(QJsonObject paramState);
    void setVeinDeviceState(QJsonObject deviceState);

    tSourceInterfaceShPtr m_ioInterface;

    cSourcePersistentJsonState* m_persistentParamState;
    cSourceJsonParamApi m_paramsRequested;
    cSourceJsonParamApi m_paramsCurrent;

    cSourceIoPacketGenerator* m_outInGenerator = nullptr;
    cSourceIoWorker m_sourceIoWorker;
    int m_currentWorkerID = 0;

    bool m_closeRequested = false;
    QUuid m_closeUuid;

    cSourceDeviceStatus  m_deviceStatus;
    cSourceVeinInterface* m_veinInterface = nullptr;

    SupportedSourceTypes m_type;
    QString m_version;


    static bool m_removeDemoByDisconnect;
    bool m_bDemoDelayFollowsTimeout = false;
};

}
#endif // CSOURCEDEVICE_H
