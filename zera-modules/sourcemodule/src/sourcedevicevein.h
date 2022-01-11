#ifndef CSOURCEDEVICE_H
#define CSOURCEDEVICE_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QUuid>
#include <QStringList>
#include "sourcedevicebase.h"
#include "sourceinterface.h"
#include "sourcedevicestatus.h"
#include "sourcepersistentjsonstate.h"

class SourceVeinInterface;

class SourceDeviceVein : public SourceDeviceBase
{
    Q_OBJECT
public:
    explicit SourceDeviceVein(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString name, QString version);
    virtual ~SourceDeviceVein();

    // requests
    bool close(QUuid uuid);
    void switchVeinLoad(QJsonObject jsonParamsState);

    // getter
    QStringList getLastErrors();

    // setter
    void setVeinInterface(SourceVeinInterface* veinInterface);
    void saveState(); // persistency

signals:
    void sigClosed(SourceDeviceVein* sourceDevice, QUuid uuid);

public slots:
    void onNewVeinParamStatus(QVariant paramState);

protected:
    virtual void handleSourceCmd(SourceWorkerCmdPack cmdPack) override;

private slots:
    void onInterfaceClosed();

private:
    void doFinalCloseActivities();
    void setVeinParamStructure(QJsonObject paramStruct);
    void setVeinParamState(QJsonObject paramState);
    void setVeinDeviceState(QJsonObject deviceState);

    SourcePersistentJsonState* m_persistentParamState;

    QUuid m_closeUuid;
    bool m_closeRequested = false;

    SourceDeviceStatus  m_deviceStatus;
    SourceVeinInterface* m_veinInterface = nullptr;

    static bool m_removeDemoByDisconnect;
};

#endif // CSOURCEDEVICE_H
