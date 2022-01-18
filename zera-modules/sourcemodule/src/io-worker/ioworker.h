#ifndef CSOURCEIOWORKER_H
#define CSOURCEIOWORKER_H

#include <QObject>
#include <QSharedPointer>
#include "iogroupgenerator.h"
#include "io-interface/iointerfacebase.h"
#include "io-device/iomultipletransfergroup.h"
#include "io-ids/ioidkeeper.h"

namespace DemoResponseHelper
{
    QList<QByteArray> generateResponseList(const IoMultipleTransferGroup &workTransferGroup);
}

class IoWorker : public QObject
{
    Q_OBJECT
public:
    explicit IoWorker(QObject *parent = nullptr);

    void setIoInterface(tIoInterfaceShPtr interface);
    void setMaxPendingGroups(int maxGroups);
    int enqueueTransferGroup(IoMultipleTransferGroup transferGroup);

    bool isIoBusy();

signals:
    void sigTransferGroupFinished(IoMultipleTransferGroup transferGroup);

private slots:
    void onIoFinished(int ioID, bool interfaceError);
    void onIoDisconnected();
signals:
    void sigTransferGroupFinishedQueued(IoMultipleTransferGroup transferGroup);
private:
    IoMultipleTransferGroup *getCurrentGroup();
    IOSingleTransferData *getNextIoTransfer();
    void tryStartNextIo();
    void finishGroup(IoMultipleTransferGroup transferGroupToFinish);
    void finishCurrentGroup();
    void abortAllGroups();
    bool evaluateResponse();
    bool canEnqueue(IoMultipleTransferGroup transferGroup);
    bool canContinueCurrentGroup();

    tIoInterfaceShPtr m_interface = nullptr;
    IoIdGenerator m_IdGenerator;
    IoIdKeeper m_currIoId;
    QList<IoMultipleTransferGroup> m_pendingGroups;
    int m_nextPosInCurrGroup = 0;
    int m_maxPendingGroups = 0;
};

#endif // CSOURCEIOWORKER_H
