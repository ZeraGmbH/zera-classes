#ifndef IOQUEUE_H
#define IOQUEUE_H

#include <QObject>
#include <QSharedPointer>
#include "ioqueuegroup.h"
#include "ioqueuegrouplistfind.h"
#include "ioqueuebehaviors.h"
#include "iodevicebase.h"
#include "idkeeper.h"

class IoQueue : public QObject
{
    Q_OBJECT
public:
    void setIoDevice(IoDeviceBase::Ptr ioDevice);
    void setMaxPendingGroups(int maxGroups);
    void enqueueTransferGroup(IoQueueGroup::Ptr transferGroup);
    bool isIoBusy() const;
signals:
    void sigTransferGroupFinished(IoQueueGroup::Ptr transferGroup);

private slots:
    void onIoFinished(int ioID, bool ioDeviceError);
    void onIoDisconnected();
private:
    IoQueueGroup::Ptr getCurrentGroup();
    IoTransferDataSingle::Ptr getNextIoTransfer();
    void connectIoDevice();
    void disconnectIoDevice();
    void tryStartNextIo();
    void finishGroup(IoQueueGroup::Ptr transferGroupToFinish);
    void finishCurrentGroup();
    void abortAllGroups();
    bool checkCurrentResponsePassed();
    bool canEnqueue(IoQueueGroup::Ptr transferGroup);
    bool canContinueCurrentGroup();
    IoDeviceBase::Ptr m_ioDevice = nullptr;
    IdKeeperSingle m_currIoId;
    IoQueueGroupListPtr m_pendingGroups;
    int m_nextPosInCurrGroup = 0;
    int m_maxPendingGroups = 0;
};

#endif // IOQUEUE_H
