#ifndef SOURCESTATEPERIODICPOLLER_H
#define SOURCESTATEPERIODICPOLLER_H

#include "abstractsourceio.h"
#include "iodevicebase.h"
#include "ioqueuegrouplistfind.h"
#include "sourcetransactionstartnotifier.h"
#include <QObject>

class test_sourceperiodicpollerstate : public QObject
{
    Q_OBJECT
public slots:
    void onIoQueueGroupFinished(IoQueueGroup::Ptr workGroup);
private slots:
    void init();

    void pollAutoStart();
    void pollStop();
    void pollRestart();
    void pollImmediatePass();
    void pollImmediateFail();
    void pollAutoStartNotification();
    void pollStopNotification();
    void pollChangeTimeNotification();
private:
    AbstractSourceIoPtr m_sourceIo;
    IoDeviceBase::Ptr m_ioDevice;
    IoQueueGroupListPtr m_listIoGroupsReceived;
    SourceTransactionStartNotifier::Ptr m_transactionNotifier;
};

#endif // SOURCESTATEPERIODICPOLLER_H
