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
    bool operator == (const cSourceIoWorkerEntry& other);
};

typedef QList<cSourceIoWorkerEntry> tSourceIoWorkerList;

class cWorkerCommandPacket
{
public:
    int m_workerId = 0;
    SourceCommandTypes m_commandType = COMMAND_UNDEFINED;
    SourcePacketErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    tSourceIoWorkerList m_workerIOList;
    bool operator == (const cWorkerCommandPacket& other);
};
Q_DECLARE_METATYPE(cWorkerCommandPacket)

class cSourceIoWorker : public QObject
{
    Q_OBJECT
public:
    explicit cSourceIoWorker(QObject *parent = nullptr);

    void setIoInterface(tSourceInterfaceShPtr interface);

    static cWorkerCommandPacket commandPackToWorkerPack(const cSourceCommandPacket &commandPack);
    int enqueueIoPacket(cWorkerCommandPacket workPack);
    bool isBusy();

signals:
    void sigWorkPackFinished(cWorkerCommandPacket packet);

private slots:
    void onIoFinished(int ioID);
    void onIoDisconnected();
signals:
    void sigWorkPackFinishedQueued(cWorkerCommandPacket pack);
private:
    void tryStartNextIo();
    cSourceIoWorkerEntry *getNextWorkerIO();
    void finishCurrentWorker();

    tSourceInterfaceShPtr m_interface = nullptr;
    int m_iCurrentIoID = 0;
    cSourceIdGenerator m_IdGenerator;
    QList<cWorkerCommandPacket> m_pendingWorkPacks;
    int m_iPositionInWorkerIo = 0;
};

#endif // CSOURCEIOWORKER_H
