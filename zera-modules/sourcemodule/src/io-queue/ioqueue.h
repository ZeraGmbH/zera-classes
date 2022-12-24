#ifndef IOQUEUE_H
#define IOQUEUE_H

#include <QObject>
#include <QSharedPointer>
#include "ioqueueentry.h"
#include "ioqueueentrylist.h"
#include "ioqueuebehaviors.h"
#include "iodevicebase.h"
#include "transaction-ids/idkeeper.h"

class IoQueue : public QObject
{
    Q_OBJECT
public:
    explicit IoQueue(QObject *parent = nullptr);
    void setIoDevice(IoDeviceBase::Ptr ioDevice);
    void setMaxPendingGroups(int maxGroups);
    int enqueueTransferGroup(IoQueueEntry::Ptr transferGroup);
    bool isIoBusy() const;
signals:
    void sigTransferGroupFinished(IoQueueEntry::Ptr transferGroup);

private slots:
    void onIoFinished(int ioID, bool ioDeviceError);
    void onIoDisconnected();
signals:
    void sigTransferGroupFinishedQueued(IoQueueEntry::Ptr transferGroup);
private:
    IoQueueEntry::Ptr getCurrentGroup();
    IoTransferDataSingle::Ptr getNextIoTransfer();
    void connectIoDevice();
    void disconnectIoDevice();
    void tryStartNextIo();
    void finishGroup(IoQueueEntry::Ptr transferGroupToFinish);
    void finishCurrentGroup();
    void abortAllGroups();
    bool checkCurrentResponsePassed();
    bool canEnqueue(IoQueueEntry::Ptr transferGroup);
    bool canContinueCurrentGroup();
    IoDeviceBase::Ptr m_ioDevice = nullptr;
    IdKeeperSingle m_currIoId;
    IoQueueEntryList m_pendingGroups;
    int m_nextPosInCurrGroup = 0;
    int m_maxPendingGroups = 0;
};

#endif // IOQUEUE_H
