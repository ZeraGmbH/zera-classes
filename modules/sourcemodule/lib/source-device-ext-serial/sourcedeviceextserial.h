#ifndef SOURCEDEVICEOBJCONTAINER_H
#define SOURCEDEVICEOBJCONTAINER_H

#include "sourcedevicetemplate.h"
#include "sourceproperties.h"
#include "sourcetransactionstartnotifier.h"
#include "sourcestatecontroller.h"
#include <QObject>
#include <QUuid>
#include <QJsonObject>

class SourceDeviceExtSerial : public SourceDeviceTemplate
{
    Q_OBJECT
public:
    SourceDeviceExtSerial(IoDeviceBase::Ptr ioDevice, SourceProperties properties);

    void setStatusPollTime(int ms) override;

    bool close(QUuid uuid) override;

private slots:
    void onSourceSwitchFinished();
    void onIoDeviceClosed();
private:
    void doFinalCloseActivities();
    void enableCloseRequested(QUuid uuid);
    bool tryDemoCloseByUsbDisconnect(QUuid uuid);
    AbstractSourceIoPtr m_sourceIo;
    SourceTransactionStartNotifier::Ptr m_transactionNotifierStatus;
    SourceTransactionStartNotifier::Ptr m_transactionNotifierSwitch;
    SourceStatePeriodicPoller::Ptr m_statePoller;
    SourceStateController m_stateController;
    QUuid m_closeUuid;
    bool m_closeRequested = false;
    static bool m_demoCloseByUsbDisconnect;
};

#endif // SOURCEDEVICEOBJCONTAINER_H
