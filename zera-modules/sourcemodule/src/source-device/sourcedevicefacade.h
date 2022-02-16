#ifndef SOURCEDEVICEOBJCONTAINER_H
#define SOURCEDEVICEOBJCONTAINER_H

#include "io-device/iodevicebase.h"
#include "sourceproperties.h"
#include "sourceio.h"
#include "sourcetransactionstartnotifier.h"
#include "sourcestatecontroller.h"
#include "sourceswitchjson.h"
#include "json/jsondevicestatusapi.h"
#include "json/persistentjsonstate.h"
#include "vein/sourceveininterface.h"

#include <QObject>
#include <QUuid>
#include <QJsonObject>
#include <QSharedDataPointer>

class SourceDeviceFacade : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<SourceDeviceFacade> Ptr;
    SourceDeviceFacade(IoDeviceBase::Ptr ioDevice, SourceProperties properties);

    void setVeinInterface(SourceVeinInterface* veinInterface);
    void setStatusPollTime(int ms);
    void switchLoad(QJsonObject params);

    QString getIoDeviceInfo() const;
    bool hasDemoIo() const;
    QStringList getLastErrors() const;

    bool close(QUuid uuid);
signals:
    void sigClosed(SourceDeviceFacade* source, QUuid uuid);

private slots:
    void onSourceStateChanged(SourceStateController::States state);
    void onSourceSwitchFinished();
    void onIoDeviceClosed();
private:
    void handleErrorState(SourceStateController::States state);
    void doFinalCloseActivities();
    void setVeinParamStructure(QJsonObject paramStruct);
    void setVeinParamState(QJsonObject paramState);
    void setVeinDeviceState(QJsonObject deviceState);
    void resetVeinComponents();
    void enableCloseRequested(QUuid uuid);
    bool tryDemoCloseByUsbDisconnect(QUuid uuid);
    SourceVeinInterface* m_veinInterface = nullptr;
    IoDeviceBase::Ptr m_ioDevice;
    ISourceIo::Ptr m_sourceIo;
    SourceTransactionStartNotifier::Ptr m_transactionNotifierStatus;
    SourceTransactionStartNotifier::Ptr m_transactionNotifierSwitch;
    SourceStatePeriodicPoller::Ptr m_statePoller;
    SourceStateController m_stateController;
    SourceSwitchJson m_switcher;
    QUuid m_closeUuid;
    bool m_closeRequested = false;
    JsonDeviceStatusApi m_deviceStatusJsonApi;
    static bool m_demoCloseByUsbDisconnect;
};

#endif // SOURCEDEVICEOBJCONTAINER_H
