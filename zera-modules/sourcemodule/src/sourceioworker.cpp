#include "sourceioworker.h"

bool SourceIoWorkerEntry::operator ==(const SourceIoWorkerEntry &other)
{
    // this for purpose of test -> ignore m_dataReceived / m_IoEval
    return m_OutIn == other.m_OutIn;
}


bool SourceWorkerCmdPack::passedAll()
{
    return m_bPassedAll;
}

void SourceWorkerCmdPack::evalAll()
{
    bool pass = true;
    for(auto io : m_workerIOList) {
        if(io.m_IoEval != SourceIoWorkerEntry::EVAL_PASS) {
            pass = false;
        }
    }
    m_bPassedAll = pass;
}

bool SourceWorkerCmdPack::operator ==(const SourceWorkerCmdPack &other)
{
    return  m_workerId == other.m_workerId &&
            m_commandType == other.m_commandType &&
            m_errorBehavior == other.m_errorBehavior &&
            m_workerIOList == other.m_workerIOList;
}


SourceWorkerCmdPack SourceWorkerConverter::commandPackToWorkerPack(const SourceCommandPacket &commandPack)
{
    SourceWorkerCmdPack workPack;
    workPack.m_commandType = commandPack.m_commandType;
    workPack.m_errorBehavior = commandPack.m_errorBehavior;
    for(auto outIn : commandPack.m_outInList) {
        SourceIoWorkerEntry workEntry;
        workEntry.m_OutIn = outIn;
        workPack.m_workerIOList.append(workEntry);
    }
    return workPack;
}

QList<QByteArray> SourceDemoHelper::generateResponseList(
        SourceWorkerCmdPack& workCmdPack, int createErrorAtIoNumber, QByteArray prefix) {
    QList<QByteArray> responseList;
    for(auto io : workCmdPack.m_workerIOList) {
        responseList.append(prefix + io.m_OutIn.m_bytesExpected);
    }
    if(createErrorAtIoNumber >= 0) {
        responseList[createErrorAtIoNumber] = "foo";
    }
    return responseList;
}

SourceIoWorker::SourceIoWorker(QObject *parent) : QObject(parent)
{
    connect(this, &SourceIoWorker::sigCmdFinishedQueued,
            this, &SourceIoWorker::sigCmdFinished, Qt::QueuedConnection);
}

void SourceIoWorker::setIoInterface(tSourceInterfaceShPtr interface)
{
    m_interface = interface;
    if(interface) {
        connect(m_interface.get(), &SourceInterfaceBase::sigIoFinished,
                this, &SourceIoWorker::onIoFinished);
        connect(m_interface.get(), &SourceInterfaceBase::sigDisconnected,
                this, &SourceIoWorker::onIoDisconnected);
    }
    else if(m_interface){
        disconnect(m_interface.get(), &SourceInterfaceBase::sigIoFinished,
                   this, &SourceIoWorker::onIoFinished);
        disconnect(m_interface.get(), &SourceInterfaceBase::sigDisconnected,
                this, &SourceIoWorker::onIoDisconnected);
        m_interface = nullptr;
    }
}

void SourceIoWorker::setMaxPendingActions(int maxPackets)
{
    m_maxPendingCmdPacks = maxPackets;
}

int SourceIoWorker::enqueueAction(SourceWorkerCmdPack cmdPack)
{
    bool canEnqueue =
            m_interface && m_interface->isOpen() &&
            !cmdPack.m_workerIOList.isEmpty() &&
            (m_maxPendingCmdPacks == 0 || m_pendingCmdPacks.size() < m_maxPendingCmdPacks);
    if(!canEnqueue) {
        finishCmd(cmdPack);
    }
    else {
        cmdPack.m_workerId = m_IdGenerator.nextID();
        m_pendingCmdPacks.append(cmdPack);
        tryStartNextIo();
    }
    return cmdPack.m_workerId;
}

bool SourceIoWorker::isIoBusy()
{
    return m_bIoIsBusy;
}

void SourceIoWorker::onIoFinished(int ioID, bool error)
{
    if(ioID == m_iCurrentIoID) {
        m_bIoIsBusy = false;
        if(error) {
            abortAllCmds();
        }
        else {
            if(canContinue()) {
                tryStartNextIo();
            }
            else {
                abortAllCmds();
            }
        }
    }
}

void SourceIoWorker::onIoDisconnected()
{
    m_bIoIsBusy = false;
    abortAllCmds();
    setIoInterface(nullptr);
}

void SourceIoWorker::tryStartNextIo()
{
    if(!isIoBusy()) {
        SourceIoWorkerEntry* workerIo = getNextIo();
        if(workerIo) {
            m_interface->setReadTimeoutNextIo(workerIo->m_OutIn.m_responseTimeoutMs);
            m_iCurrentIoID = m_interface->sendAndReceive(
                        workerIo->m_OutIn.m_bytesSend,
                        &workerIo->m_dataReceived);
            m_bIoIsBusy = true;
        }
    }
}

SourceIoWorkerEntry* SourceIoWorker::getNextIo()
{
    SourceIoWorkerEntry* workerIo = nullptr;
    SourceWorkerCmdPack *currCmdPack = getCurrentCmd();
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

void SourceIoWorker::finishCurrentCmd()
{
    m_nextPosInWorkerIo = 0;
    finishCmd(m_pendingCmdPacks.takeFirst());
}

void SourceIoWorker::finishCmd(SourceWorkerCmdPack cmdToFinish)
{
    cmdToFinish.evalAll();
    emit sigCmdFinishedQueued(cmdToFinish);
}

void SourceIoWorker::abortAllCmds()
{
    while(!m_pendingCmdPacks.isEmpty()) {
        finishCurrentCmd();
    }
}

bool SourceIoWorker::evaluateResponse()
{
    bool pass = false;
    SourceWorkerCmdPack *currCmdPack = getCurrentCmd();
    if(currCmdPack) {
        SourceIoWorkerEntry& currentWorker = currCmdPack->m_workerIOList[m_nextPosInWorkerIo-1];
        SourceSingleOutIn& currentOutIn = currentWorker.m_OutIn;
        switch(currentOutIn.m_responseType) {
        case RESP_FULL_DATA_SEQUENCE:
            currentWorker.m_IoEval = currentWorker.m_dataReceived == currentOutIn.m_bytesExpected ?
                        SourceIoWorkerEntry::EVAL_PASS : SourceIoWorkerEntry::EVAL_FAIL;
            break;
        case RESP_PART_DATA_SEQUENCE:
            currentWorker.m_IoEval = currentWorker.m_dataReceived.contains(currentOutIn.m_bytesExpected) ?
                        SourceIoWorkerEntry::EVAL_PASS : SourceIoWorkerEntry::EVAL_FAIL;
            break;
        default:
            currentWorker.m_IoEval = SourceIoWorkerEntry::EVAL_FAIL;
            break;
        }
        pass = currentWorker.m_IoEval == SourceIoWorkerEntry::EVAL_PASS;
    }
    return pass;
}

bool SourceIoWorker::canContinue()
{
    bool pass = evaluateResponse();
    SourceWorkerCmdPack *currCmdPack = getCurrentCmd();
    return pass || (currCmdPack && currCmdPack->m_errorBehavior == BEHAVE_CONTINUE_ON_ERROR);
}

SourceWorkerCmdPack *SourceIoWorker::getCurrentCmd()
{
    SourceWorkerCmdPack* current = nullptr;
    if(!m_pendingCmdPacks.isEmpty()) {
        current = &m_pendingCmdPacks.first();
    }
    return current;
}
