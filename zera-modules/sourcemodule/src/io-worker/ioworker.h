#ifndef CSOURCEIOWORKER_H
#define CSOURCEIOWORKER_H

#include <QObject>
#include <QSharedPointer>
#include "iogroupgenerator.h"
#include "io-device/iodevicebaseserial.h"
#include "io-device/iotransferdatagroup.h"
#include "io-ids/ioidkeeper.h"

class IoWorker : public QObject
{
    Q_OBJECT
public:
    explicit IoWorker(QObject *parent = nullptr);

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
    tIoTransferDataSingleShPtr getNextIoTransfer();
    void tryStartNextIo();
    void finishGroup(IoTransferDataGroup transferGroupToFinish);
    void finishCurrentGroup();
    void abortAllGroups();
    bool evaluateResponse();
    bool canEnqueue(IoTransferDataGroup transferGroup);
    bool canContinueCurrentGroup();

    tIoDeviceShPtr m_interface = nullptr;
    IoIdGenerator m_IdGenerator;
    IoIdKeeper m_currIoId;
    QList<IoTransferDataGroup> m_pendingGroups;
    int m_nextPosInCurrGroup = 0;
    int m_maxPendingGroups = 0;
};

#endif // CSOURCEIOWORKER_H
