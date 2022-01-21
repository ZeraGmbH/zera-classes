#ifndef IOQUEUE_H
#define IOQUEUE_H

#include <QObject>
#include <QSharedPointer>
#include "iogroupgenerator.h"
#include "io-device/iodevicebrokendummy.h"
#include "io-device/iotransferdatagroup.h"
#include "io-ids/ioidkeeper.h"

class IoQueue : public QObject
{
    Q_OBJECT
public:
    explicit IoQueue(QObject *parent = nullptr);

    void setIoInterface(tIoDeviceShPtr interface);
    void setMaxPendingGroups(int maxGroups);
    int enqueueTransferGroup(IoTransferDataGroup transferGroup);

    bool isIoBusy() const;

signals:
    void sigTransferGroupFinished(IoTransferDataGroup transferGroup);

private slots:
    void onIoFinished(int ioID, bool ioDeviceError);
    void onIoDisconnected();
signals:
    void sigTransferGroupFinishedQueued(IoTransferDataGroup transferGroup);
private:
    IoTransferDataGroup *getCurrentGroup();
    IoTransferDataSingle::Ptr getNextIoTransfer();
    void tryStartNextIo();
    void finishGroup(IoTransferDataGroup transferGroupToFinish);
    void finishCurrentGroup();
    void abortAllGroups();
    bool checkCurrentResponsePassed();
    bool canEnqueue(IoTransferDataGroup transferGroup);
    bool canContinueCurrentGroup();

    tIoDeviceShPtr m_interface = nullptr;
    IoIdKeeper m_currIoId;
    QList<IoTransferDataGroup> m_pendingGroups;
    int m_nextPosInCurrGroup = 0;
    int m_maxPendingGroups = 0;
};

#endif // IOQUEUE_H
