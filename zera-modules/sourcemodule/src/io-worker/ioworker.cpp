#include "ioworker.h"

bool IoWorkerEntry::operator ==(const IoWorkerEntry &other)
{
    // this for purpose of test -> ignore m_dataReceived / m_IoEval
    return m_OutIn == other.m_OutIn;
}


bool IoWorkerCmdPack::passedAll() const
{
    return m_bPassedAll;
}

void IoWorkerCmdPack::evalAll()
{
    bool pass = true;
    for(auto io : m_workerIOList) {
        if(io.m_IoEval != IoWorkerEntry::EVAL_PASS) {
            pass = false;
        }
    }
    m_bPassedAll = pass;
}

bool IoWorkerCmdPack::isSwitchPack() const
{
    return m_commandType == COMMAND_SWITCH_ON || m_commandType == COMMAND_SWITCH_OFF;
}

bool IoWorkerCmdPack::isStateQuery() const
{
    return m_commandType == COMMAND_STATE_POLL;
}

bool IoWorkerCmdPack::operator ==(const IoWorkerCmdPack &other)
{
    return  m_workerId == other.m_workerId &&
            m_commandType == other.m_commandType &&
            m_errorBehavior == other.m_errorBehavior &&
            m_workerIOList == other.m_workerIOList;
}


IoWorkerCmdPack IoWorkerConverter::commandPackToWorkerPack(const IoCommandPacket &commandPack)
{
    IoWorkerCmdPack workPack;
    workPack.m_commandType = commandPack.m_commandType;
    workPack.m_errorBehavior = commandPack.m_errorBehavior;
    for(auto outIn : commandPack.m_outInList) {
        IoWorkerEntry workEntry;
        workEntry.m_OutIn = outIn;
        workPack.m_workerIOList.append(workEntry);
    }
    return workPack;
}

QList<QByteArray> DemoResponseHelper::generateResponseList(const IoWorkerCmdPack& workCmdPack) {
    QList<QByteArray> responseList;
    for(auto io : workCmdPack.m_workerIOList) {
        responseList.append(io.m_OutIn.m_bytesExpectedLead + io.m_OutIn.m_bytesExpectedTrail);
    }
    return responseList;
}

IoWorker::IoWorker(QObject *parent) : QObject(parent)
{
    connect(this, &IoWorker::sigCmdFinishedQueued,
            this, &IoWorker::sigCmdFinished, Qt::QueuedConnection);
}

void IoWorker::setIoInterface(tIoInterfaceShPtr interface)
{
    m_interface = interface;
    if(interface) {
        connect(m_interface.get(), &IoInterfaceBase::sigIoFinished,
                this, &IoWorker::onIoFinished);
        connect(m_interface.get(), &IoInterfaceBase::sigDisconnected,
                this, &IoWorker::onIoDisconnected);
    }
    else if(m_interface){
        disconnect(m_interface.get(), &IoInterfaceBase::sigIoFinished,
                   this, &IoWorker::onIoFinished);
        disconnect(m_interface.get(), &IoInterfaceBase::sigDisconnected,
                this, &IoWorker::onIoDisconnected);
        m_interface = nullptr;
    }
}

void IoWorker::setMaxPendingCmds(int maxPackets)
{
    m_maxPendingCmdPacks = maxPackets;
}

int IoWorker::enqueueCmd(IoWorkerCmdPack cmdPack)
{
    if(!canEnqueue(cmdPack)) {
        finishCmd(cmdPack);
    }
    else {
        cmdPack.m_workerId = m_IdGenerator.nextID();
        m_pendingCmdPacks.append(cmdPack);
        tryStartNextIo();
    }
    return cmdPack.m_workerId;
}

bool IoWorker::isIoBusy()
{
    return m_currIoId.isActive();
}

void IoWorker::onIoFinished(int ioID, bool interfaceError)
{
    if(m_currIoId.isCurrAndDeactivateIf(ioID)) {
        if(interfaceError) {
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

void IoWorker::onIoDisconnected()
{
    m_currIoId.deactivate();
    abortAllCmds();
    setIoInterface(nullptr);
}

void IoWorker::tryStartNextIo()
{
    if(!isIoBusy()) {
        IoWorkerEntry* workerIo = getNextIo();
        if(workerIo) {
            m_interface->setReadTimeoutNextIo(workerIo->m_OutIn.m_responseTimeoutMs);
            int ioId = m_interface->sendAndReceive(workerIo->m_OutIn.m_bytesSend,
                                                   &workerIo->m_dataReceived);
            m_currIoId.setCurrent(ioId);
        }
    }
}

IoWorkerEntry* IoWorker::getNextIo()
{
    IoWorkerEntry* workerIo = nullptr;
    IoWorkerCmdPack *currCmdPack = getCurrentCmd();
    if(currCmdPack) {
        if(m_nextPosInCurrCmd < currCmdPack->m_workerIOList.count()) {
            workerIo = &(currCmdPack->m_workerIOList[m_nextPosInCurrCmd]);
            m_nextPosInCurrCmd++;
        }
        else {
            finishCurrentCmd();
            workerIo = getNextIo();
        }
    }
    return workerIo;
}

void IoWorker::finishCurrentCmd()
{
    m_nextPosInCurrCmd = 0;
    finishCmd(m_pendingCmdPacks.takeFirst());
}

void IoWorker::finishCmd(IoWorkerCmdPack cmdPackToFinish)
{
    cmdPackToFinish.evalAll();
    emit sigCmdFinishedQueued(cmdPackToFinish);
}

void IoWorker::abortAllCmds()
{
    while(!m_pendingCmdPacks.isEmpty()) {
        finishCurrentCmd();
    }
}

bool IoWorker::evaluateResponse()
{
    bool pass = false;
    IoWorkerCmdPack *currCmdPack = getCurrentCmd();
    if(currCmdPack) {
        IoWorkerEntry& currentWorker = currCmdPack->m_workerIOList[m_nextPosInCurrCmd-1];
        IoSingleOutIn& currentOutIn = currentWorker.m_OutIn;
        if(currentWorker.m_dataReceived.isEmpty()) {
            currentWorker.m_IoEval = IoWorkerEntry::EVAL_NO_ANSWER;
        }
        else {
            pass =
                    currentWorker.m_dataReceived.startsWith(currentOutIn.m_bytesExpectedLead) &&
                    currentWorker.m_dataReceived.endsWith(currentOutIn.m_bytesExpectedTrail);
            currentWorker.m_IoEval = pass ? IoWorkerEntry::EVAL_PASS : IoWorkerEntry::EVAL_WRONG_ANSWER;
        }
    }
    return pass;
}

bool IoWorker::canEnqueue(IoWorkerCmdPack cmdPack)
{
    bool canEnqueue =
            m_interface && m_interface->isOpen() &&
            !cmdPack.m_workerIOList.isEmpty() &&
            (m_maxPendingCmdPacks == 0 || m_pendingCmdPacks.size() < m_maxPendingCmdPacks);
    return canEnqueue;
}

bool IoWorker::canContinueCurrentCmd()
{
    bool pass = evaluateResponse();
    IoWorkerCmdPack *currCmdPack = getCurrentCmd();
    return pass || (currCmdPack && currCmdPack->m_errorBehavior == BEHAVE_CONTINUE_ON_ERROR);
}

IoWorkerCmdPack *IoWorker::getCurrentCmd()
{
    IoWorkerCmdPack* current = nullptr;
    if(!m_pendingCmdPacks.isEmpty()) {
        current = &m_pendingCmdPacks.first();
    }
    return current;
}
