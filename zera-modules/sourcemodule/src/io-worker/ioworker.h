#ifndef CSOURCEIOWORKER_H
#define CSOURCEIOWORKER_H

#include <QObject>
#include <QSharedPointer>
#include "iopacketgenerator.h"
#include "io-interface/iointerfacebase.h"
#include "io-ids/ioidkeeper.h"

class IoWorkerEntry
{
public:
    IoSingleOutIn m_OutIn;
    QByteArray m_dataReceived;
    enum {
        EVAL_UNKNOWN = 0,
        EVAL_NO_ANSWER,
        EVAL_WRONG_ANSWER,
        EVAL_PASS
    } m_IoEval = EVAL_UNKNOWN;
    bool operator == (const IoWorkerEntry& other);
};

typedef QList<IoWorkerEntry> tIoWorkerList;


class IoWorkerCmdPack
{
public:
    bool passedAll() const;
    void evalAll();

    bool isSwitchPack() const;
    bool isStateQuery() const;

    bool operator == (const IoWorkerCmdPack& other);

    int m_workerId = 0;
    SourceCommandTypes m_commandType = COMMAND_UNDEFINED;
    PacketErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    tIoWorkerList m_workerIOList;
private:
    bool m_bPassedAll = false;
};
Q_DECLARE_METATYPE(IoWorkerCmdPack)


namespace IoWorkerConverter
{
    IoWorkerCmdPack commandPackToWorkerPack(const IoCommandPacket &cmdPack);
}

namespace DemoResponseHelper
{
    QList<QByteArray> generateResponseList(const IoWorkerCmdPack &workCmdPack);
}

class IoWorker : public QObject
{
    Q_OBJECT
public:
    explicit IoWorker(QObject *parent = nullptr);

    void setIoInterface(tIoInterfaceShPtr interface);
    void setMaxPendingCmds(int maxPackets);
    int enqueueCmd(IoWorkerCmdPack cmdPack);

    bool isIoBusy();

signals:
    void sigCmdFinished(IoWorkerCmdPack cmdPack);

private slots:
    void onIoFinished(int ioID, bool interfaceError);
    void onIoDisconnected();
signals:
    void sigCmdFinishedQueued(IoWorkerCmdPack cmdPack);
private:
    IoWorkerCmdPack *getCurrentCmd();
    IoWorkerEntry *getNextIo();
    void tryStartNextIo();
    void finishCmd(IoWorkerCmdPack cmdPackToFinish);
    void finishCurrentCmd();
    void abortAllCmds();
    bool evaluateResponse();
    bool canEnqueue(IoWorkerCmdPack cmdPack);
    bool canContinueCurrentCmd();

    tIoInterfaceShPtr m_interface = nullptr;
    IoIdGenerator m_IdGenerator;
    IoIdKeeper m_currIoId;
    QList<IoWorkerCmdPack> m_pendingCmdPacks;
    int m_nextPosInCurrCmd = 0;
    int m_maxPendingCmdPacks = 0;
};

#endif // CSOURCEIOWORKER_H
