#include "sourceioworker.h"

bool cSourceIoWorkerEntry::operator ==(const cSourceIoWorkerEntry &other)
{
    // this for purpose of test -> ignore m_dataReceived / m_IoEval
    return m_OutIn == other.m_OutIn;
}


bool cWorkerCommandPacket::passedAll()
{
    return m_bPassedAll;
}

void cWorkerCommandPacket::evalAll()
{
    bool pass = true;
    for(auto io : m_workerIOList) {
        if(io.m_IoEval != cSourceIoWorkerEntry::EVAL_PASS) {
            pass = false;
        }
    }
    m_bPassedAll = pass;
}

bool cWorkerCommandPacket::operator ==(const cWorkerCommandPacket &other)
{
    return  m_workerId == other.m_workerId &&
            m_commandType == other.m_commandType &&
            m_errorBehavior == other.m_errorBehavior &&
            m_workerIOList == other.m_workerIOList;
}


cWorkerCommandPacket SourceWorkerConverter::commandPackToWorkerPack(const cSourceCommandPacket &commandPack)
{
    cWorkerCommandPacket workPack;
    workPack.m_commandType = commandPack.m_commandType;
    workPack.m_errorBehavior = commandPack.m_errorBehavior;
    for(auto outIn : commandPack.m_outInList) {
        cSourceIoWorkerEntry workEntry;
        workEntry.m_OutIn = outIn;
        workPack.m_workerIOList.append(workEntry);
    }
    return workPack;
}

QList<QByteArray> SourceDemoHelper::generateResponseList(
        cWorkerCommandPacket& workCmdPack, int createErrorAtIoNumber, QByteArray prefix) {
    QList<QByteArray> responseList;
    for(auto io : workCmdPack.m_workerIOList) {
        responseList.append(prefix + io.m_OutIn.m_bytesExpected);
    }
    if(createErrorAtIoNumber >= 0) {
        responseList[createErrorAtIoNumber] = "foo";
    }
    return responseList;
}

cSourceIoWorker::cSourceIoWorker(QObject *parent) : QObject(parent)
{
    connect(this, &cSourceIoWorker::sigCmdFinishedQueued,
            this, &cSourceIoWorker::sigCmdFinished, Qt::QueuedConnection);
}

void cSourceIoWorker::setIoInterface(tSourceInterfaceShPtr interface)
{
    m_interface = interface;
    if(interface) {
        connect(m_interface.get(), &cSourceInterfaceBase::sigIoFinished,
                this, &cSourceIoWorker::onIoFinished);
        connect(m_interface.get(), &cSourceInterfaceBase::sigDisconnected,
                this, &cSourceIoWorker::onIoDisconnected);
    }
    else if(m_interface){
        disconnect(m_interface.get(), &cSourceInterfaceBase::sigIoFinished,
                   this, &cSourceIoWorker::onIoFinished);
        disconnect(m_interface.get(), &cSourceInterfaceBase::sigDisconnected,
                this, &cSourceIoWorker::onIoDisconnected);
        m_interface = nullptr;
    }
}

void cSourceIoWorker::setMaxPendingActions(int maxPackets)
{
    m_maxPendingCmdPacks = maxPackets;
}

int cSourceIoWorker::enqueueAction(cWorkerCommandPacket cmdPack)
{
    bool canEnqueue =
            m_interface &&
            !cmdPack.m_workerIOList.isEmpty() &&
            (m_maxPendingCmdPacks == 0 || m_pendingWorkPacks.size() < m_maxPendingCmdPacks);
    if(!canEnqueue) {
        finishCmd(cmdPack);
    }
    else {
        cmdPack.m_workerId = m_IdGenerator.nextID();
        m_pendingWorkPacks.append(cmdPack);
        tryStartNextIo();
    }
    return cmdPack.m_workerId;
}

bool cSourceIoWorker::isBusy()
{
    return m_iCurrentIoID != 0;
}

void cSourceIoWorker::onIoFinished(int ioID)
{
    if(ioID == 0) {
        abortAllCmds();
    }
    else {
        m_iCurrentIoID = 0;
        if(canContinue()) {
            tryStartNextIo();
        }
        else {
            abortAllCmds();
        }
    }
}

void cSourceIoWorker::onIoDisconnected()
{
    abortAllCmds();
    setIoInterface(nullptr);
}

void cSourceIoWorker::tryStartNextIo()
{
    if(!isBusy()) {
        cSourceIoWorkerEntry* workerIo = getNextIo();
        if(workerIo) {
            m_iCurrentIoID = m_interface->sendAndReceive(
                        workerIo->m_OutIn.m_bytesSend,
                        &workerIo->m_dataReceived);
        }
    }
}

cSourceIoWorkerEntry* cSourceIoWorker::getNextIo()
{
    cSourceIoWorkerEntry* workerIo = nullptr;
    cWorkerCommandPacket *currCmdPack = getCurrentActionPack();
    if(currCmdPack) {
        if(m_nextPosInWorkerIo < currCmdPack->m_workerIOList.count()) {
            workerIo = &(currCmdPack->m_workerIOList[m_nextPosInWorkerIo]);
            m_nextPosInWorkerIo++;
        }
        else {
            finishCurrentCmd();
            workerIo = getNextIo();
        }
    }
    return workerIo;
}

void cSourceIoWorker::finishCurrentCmd()
{
    m_nextPosInWorkerIo = 0;
    m_iCurrentIoID = 0;
    finishCmd(m_pendingWorkPacks.takeFirst());
}

void cSourceIoWorker::finishCmd(cWorkerCommandPacket cmdToFinish)
{
    cmdToFinish.evalAll();
    emit sigCmdFinishedQueued(cmdToFinish);
}

void cSourceIoWorker::abortAllCmds()
{
    while(!m_pendingWorkPacks.isEmpty()) {
        finishCurrentCmd();
    }
}

bool cSourceIoWorker::evaluateResponse()
{
    bool pass = false;
    cWorkerCommandPacket *currCmdPack = getCurrentActionPack();
    if(currCmdPack) {
        cSourceIoWorkerEntry& currentWorker = currCmdPack->m_workerIOList[m_nextPosInWorkerIo-1];
        cSourceSingleOutIn& currentOutIn = currentWorker.m_OutIn;
        switch(currentOutIn.m_responseType) {
        case RESP_FULL_DATA_SEQUENCE:
            currentWorker.m_IoEval = currentWorker.m_dataReceived == currentOutIn.m_bytesExpected ?
                        cSourceIoWorkerEntry::EVAL_PASS : cSourceIoWorkerEntry::EVAL_FAIL;
            break;
        case RESP_PART_DATA_SEQUENCE:
            currentWorker.m_IoEval = currentWorker.m_dataReceived.contains(currentOutIn.m_bytesExpected) ?
                        cSourceIoWorkerEntry::EVAL_PASS : cSourceIoWorkerEntry::EVAL_FAIL;
            break;
        default:
            currentWorker.m_IoEval = cSourceIoWorkerEntry::EVAL_FAIL;
            break;
        }
        pass = currentWorker.m_IoEval == cSourceIoWorkerEntry::EVAL_PASS;
    }
    return pass;
}

bool cSourceIoWorker::canContinue()
{
    bool pass = evaluateResponse();
    cWorkerCommandPacket *currCmdPack = getCurrentActionPack();
    return pass || (currCmdPack && currCmdPack->m_errorBehavior == BEHAVE_CONTINUE_ON_ERROR);
}

cWorkerCommandPacket *cSourceIoWorker::getCurrentActionPack()
{
    cWorkerCommandPacket* current = nullptr;
    if(!m_pendingWorkPacks.isEmpty()) {
        current = &m_pendingWorkPacks.first();
    }
    return current;
}
