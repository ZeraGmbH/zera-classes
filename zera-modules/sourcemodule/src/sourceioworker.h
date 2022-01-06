#ifndef CSOURCEIOWORKER_H
#define CSOURCEIOWORKER_H

#include <QObject>
#include <QSharedPointer>
#include "sourceiopacketgenerator.h"
#include "sourceinterface.h"

class SourceIoWorkerEntry
{
public:
    SourceSingleOutIn m_OutIn;
    QByteArray m_dataReceived;
    enum {
        EVAL_UNKNOWN = 0,
        EVAL_FAIL,
        EVAL_PASS
    } m_IoEval = EVAL_UNKNOWN;
    bool operator == (const SourceIoWorkerEntry& other);
};

typedef QList<SourceIoWorkerEntry> tSourceIoWorkerList;


class SourceWorkerCmdPack
{
public:
    bool passedAll();
    void evalAll();

    bool operator == (const SourceWorkerCmdPack& other);

    int m_workerId = 0;
    SourceCommandTypes m_commandType = COMMAND_UNDEFINED;
    SourcePacketErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    tSourceIoWorkerList m_workerIOList;
private:
    bool m_bPassedAll = false;
};
Q_DECLARE_METATYPE(SourceWorkerCmdPack)


namespace SourceWorkerConverter
{
    SourceWorkerCmdPack commandPackToWorkerPack(const SourceCommandPacket &cmdPack);
}

namespace SourceDemoHelper
{
    QList<QByteArray> generateResponseList(
            SourceWorkerCmdPack& workCmdPack,
            int createErrorAtIoNumber = -1,
            QByteArray prefix = "");
}

class SourceIoWorker : public QObject
{
    Q_OBJECT
public:
    explicit SourceIoWorker(QObject *parent = nullptr);

    void setIoInterface(tSourceInterfaceShPtr interface);
    void setMaxPendingActions(int maxPackets);
    int enqueueAction(SourceWorkerCmdPack cmdPack);

    bool isIoBusy();

signals:
    void sigCmdFinished(SourceWorkerCmdPack cmdPack);

private slots:
    void onIoFinished(int ioID, bool error);
    void onIoDisconnected();
signals:
    void sigCmdFinishedQueued(SourceWorkerCmdPack cmdPack);
private:
    SourceWorkerCmdPack *getCurrentCmd();
    SourceIoWorkerEntry *getNextIo();
    void tryStartNextIo();
    void finishCmd(SourceWorkerCmdPack cmdToFinish);
    void finishCurrentCmd();
    void abortAllCmds();
    bool evaluateResponse();
    bool canContinue();

    tSourceInterfaceShPtr m_interface = nullptr;
    int m_iCurrentIoID = 0;
    bool m_bIoIsBusy = false;
    SourceIdGenerator m_IdGenerator;
    QList<SourceWorkerCmdPack> m_pendingCmdPacks;
    int m_nextPosInWorkerIo = 0;
    int m_maxPendingCmdPacks = 0;
};

#endif // CSOURCEIOWORKER_H
