#ifndef CSOURCEIOWORKER_H
#define CSOURCEIOWORKER_H

#include <QObject>
#include <QSharedPointer>
#include "sourceiopacketgenerator.h"
#include "sourceinterface.h"

class cSourceIoWorkerEntry
{
public:
    cSourceSingleOutIn m_OutIn;
    QByteArray m_dataReceived;
    enum {
        EVAL_UNKNOWN = 0,
        EVAL_FAIL,
        EVAL_PASS
    } m_IoEval = EVAL_UNKNOWN;
};

typedef QList<cSourceIoWorkerEntry> tSourceIoWorkerList;

class cWorkerPacket
{
public:
    int m_workerId = 0;
    SourceCommandTypes m_commandType = COMMAND_UNDEFINED;
    SourcePacketErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    tSourceIoWorkerList m_workerList;
};

class cSourceIoWorker : public QObject
{
    Q_OBJECT
public:
    explicit cSourceIoWorker(QObject *parent = nullptr);

    void setIoInterface(tSourceInterfaceShPtr interface);

    static cWorkerPacket commandPackToWorkerPack(const cSourceCommandPacket &commandPack);
    int enqueueIoPacket(cWorkerPacket workPack);
    bool isBusy();

signals:
    void workPackFinished(cWorkerPacket packet);

private slots:
    void onIoFinished(int ioID);
    void onIoDisconnected();
signals:
    void workPackFinishedQueued(cWorkerPacket pack);
private:
    void tryStartNextIo();
    cSourceSingleOutIn getNextOutIn();

    tSourceInterfaceShPtr m_interface = nullptr;
    int m_iCurrentIoID = 0;
    cSourceIdGenerator m_IdGenerator;
    QList<cWorkerPacket> m_pendingWorkPacks;
    cWorkerPacket m_currentWorkPack;
};

#endif // CSOURCEIOWORKER_H
