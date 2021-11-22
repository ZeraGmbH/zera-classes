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


class cSourceWorkerConverter
{
public:
    static cWorkerCommandPacket commandPackToWorkerPack(const cSourceCommandPacket &commandPack);
};


class cSourceIoWorker : public QObject
{
    Q_OBJECT
public:
    explicit cSourceIoWorker(QObject *parent = nullptr);

    void setIoInterface(tSourceInterfaceShPtr interface);
    int enqueuePacket(cWorkerCommandPacket workPack);
    bool isBusy();

signals:
    void sigPackFinished(cWorkerCommandPacket packet);

private slots:
    void onIoFinished(int ioID);
    void onIoDisconnected();
signals:
    void sigPackFinishedQueued(cWorkerCommandPacket pack);
private:
    cWorkerCommandPacket *getCurrentCmdPack();
    cSourceIoWorkerEntry *getNextIo();
    void tryStartNextIo();
    void finishCurrentWorker();
    void abortAllWorkers();
    bool evaluateResponse();
    bool canContinue();

    tSourceInterfaceShPtr m_interface = nullptr;
    int m_iCurrentIoID = 0;
    cSourceIdGenerator m_IdGenerator;
    QList<cWorkerCommandPacket> m_pendingWorkPacks;
    int m_nextPosInWorkerIo = 0;
};

#endif // CSOURCEIOWORKER_H
