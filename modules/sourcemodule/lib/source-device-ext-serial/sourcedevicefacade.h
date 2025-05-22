#ifndef SOURCEDEVICEOBJCONTAINER_H
#define SOURCEDEVICEOBJCONTAINER_H

#include "sourcedevicefacadetemplate.h"
#include "sourceproperties.h"
#include "sourceio.h"
#include "sourcetransactionstartnotifier.h"
#include "sourcestatecontroller.h"
#include "sourceswitchjson.h"
#include "sourceveininterface.h"

#include <QObject>
#include <QUuid>
#include <QJsonObject>
#include <QSharedDataPointer>

class SourceDeviceFacade : public SourceDeviceFacadeTemplate
{
    Q_OBJECT
public:
    typedef QSharedPointer<SourceDeviceFacade> Ptr;
    SourceDeviceFacade(IoDeviceBase::Ptr ioDevice, SourceProperties properties);

    void setVeinInterface(SourceVeinInterface* veinInterface);
    void setStatusPollTime(int ms);
    void switchLoad(QJsonObject params);

    bool close(QUuid uuid);

private slots:
    void onSourceStateChanged(SourceStateController::States state);
    void onSourceSwitchFinished();
    void onIoDeviceClosed();
private:
    void handleErrorState(SourceStateController::States state);
    void doFinalCloseActivities();
    void enableCloseRequested(QUuid uuid);
    bool tryDemoCloseByUsbDisconnect(QUuid uuid);
    SourceTransactionStartNotifier::Ptr m_transactionNotifierStatus;
    SourceTransactionStartNotifier::Ptr m_transactionNotifierSwitch;
    SourceStatePeriodicPoller::Ptr m_statePoller;
    SourceStateController m_stateController;
    SourceSwitchJson m_switcher;
    QUuid m_closeUuid;
    bool m_closeRequested = false;
    static bool m_demoCloseByUsbDisconnect;
};

#endif // SOURCEDEVICEOBJCONTAINER_H
