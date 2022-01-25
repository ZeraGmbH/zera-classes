#ifndef IOQUEUE_H
#define IOQUEUE_H

#include <QObject>
#include <QSharedPointer>
#include "io-device/iodevicebase.h"
#include "io-device/iotransferdatagroup.h"
#include "transaction-ids/idkeeper.h"

class IoQueue : public QObject
{
    Q_OBJECT
public:
    explicit IoQueue(QObject *parent = nullptr);

    void setIoDevice(IoDeviceBase::Ptr ioDevice);
    void setMaxPendingGroups(int maxGroups);
    int enqueueTransferGroup(IoTransferDataGroup::Ptr transferGroup);

    bool isIoBusy() const;

signals:
    void sigTransferGroupFinished(IoTransferDataGroup::Ptr transferGroup);

private slots:
    void onIoFinished(int ioID, bool ioDeviceError);
    void onIoDisconnected();
signals:
    void sigTransferGroupFinishedQueued(IoTransferDataGroup::Ptr transferGroup);
private:
    IoTransferDataGroup::Ptr getCurrentGroup();
    IoTransferDataSingle::Ptr getNextIoTransfer();
    void connectIoDevice();
    void disconnectIoDevice();
    void tryStartNextIo();
    void finishGroup(IoTransferDataGroup::Ptr transferGroupToFinish);
    void finishCurrentGroup();
    void abortAllGroups();
    bool checkCurrentResponsePassed();
    bool canEnqueue(IoTransferDataGroup::Ptr transferGroup);
    bool canContinueCurrentGroup();

    IoDeviceBase::Ptr m_ioDevice = nullptr;
    IdKeeperSingle m_currIoId;
    QList<IoTransferDataGroup::Ptr> m_pendingGroups;
    int m_nextPosInCurrGroup = 0;
    int m_maxPendingGroups = 0;
};

#endif // IOQUEUE_H
