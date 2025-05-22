#ifndef SOURCEDEVICEOBJCONTAINER_H
#define SOURCEDEVICEOBJCONTAINER_H

#include "sourcedevicefacadetemplate.h"
#include "sourceproperties.h"
#include "sourcetransactionstartnotifier.h"
#include "sourcestatecontroller.h"
#include <QObject>
#include <QUuid>
#include <QJsonObject>
#include <memory>

class SourceDeviceFacade : public SourceDeviceFacadeTemplate
{
    Q_OBJECT
public:
    typedef std::shared_ptr<SourceDeviceFacade> Ptr;
    SourceDeviceFacade(IoDeviceBase::Ptr ioDevice, SourceProperties properties);

    void setStatusPollTime(int ms) override;

    bool close(QUuid uuid) override;

private slots:
    void onSourceSwitchFinished();
    void onIoDeviceClosed();
private:
    void doFinalCloseActivities();
    void enableCloseRequested(QUuid uuid);
    bool tryDemoCloseByUsbDisconnect(QUuid uuid);
    SourceTransactionStartNotifier::Ptr m_transactionNotifierStatus;
    SourceTransactionStartNotifier::Ptr m_transactionNotifierSwitch;
    SourceStatePeriodicPoller::Ptr m_statePoller;
    SourceStateController m_stateController;
    QUuid m_closeUuid;
    bool m_closeRequested = false;
    static bool m_demoCloseByUsbDisconnect;
};

#endif // SOURCEDEVICEOBJCONTAINER_H
