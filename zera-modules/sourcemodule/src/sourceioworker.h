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
    bool passedAll();
    void evalAll();

    bool operator == (const cWorkerCommandPacket& other);

    int m_workerId = 0;
    SourceCommandTypes m_commandType = COMMAND_UNDEFINED;
    SourcePacketErrorBehaviors m_errorBehavior = BEHAVE_UNDEFINED;
    tSourceIoWorkerList m_workerIOList;
private:
    bool m_bPassedAll = false;
};
Q_DECLARE_METATYPE(cWorkerCommandPacket)


namespace SourceWorkerConverter
{
    cWorkerCommandPacket commandPackToWorkerPack(const cSourceCommandPacket &cmdPack);
}

namespace SourceDemoHelper
{
    QList<QByteArray> generateResponseList(
            cWorkerCommandPacket& workCmdPack,
            int createErrorAtIoNumber = -1,
            QByteArray prefix = "");
}

class cSourceIoWorker : public QObject
{
    Q_OBJECT
public:
    explicit cSourceIoWorker(QObject *parent = nullptr);

    void setIoInterface(tSourceInterfaceShPtr interface);
    void setMaxPendingActions(int maxPackets);
    int enqueueAction(cWorkerCommandPacket cmdPack);

    bool isIoBusy();

signals:
    void sigCmdFinished(cWorkerCommandPacket cmdPack);

private slots:
    void onIoFinished(int ioID, bool error);
    void onIoDisconnected();
signals:
    void sigCmdFinishedQueued(cWorkerCommandPacket cmdPack);
private:
    cWorkerCommandPacket *getCurrentCmd();
    cSourceIoWorkerEntry *getNextIo();
    void tryStartNextIo();
    void finishCmd(cWorkerCommandPacket cmdToFinish);
    void finishCurrentCmd();
    void abortAllCmds();
    bool evaluateResponse();
    bool canContinue();

    tSourceInterfaceShPtr m_interface = nullptr;
    int m_iCurrentIoID = 0;
    bool m_bIoIsBusy = false;
    cSourceIdGenerator m_IdGenerator;
    QList<cWorkerCommandPacket> m_pendingCmdPacks;
    int m_nextPosInWorkerIo = 0;
    int m_maxPendingCmdPacks = 0;
};

#endif // CSOURCEIOWORKER_H
