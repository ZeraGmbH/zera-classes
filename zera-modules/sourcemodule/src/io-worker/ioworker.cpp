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

bool IoWorkerCmdPack::isSwitchPack()
{
    return m_commandType == COMMAND_SWITCH_ON || m_commandType == COMMAND_SWITCH_OFF;
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

QList<QByteArray> DemoResponseHelper::generateResponseList(
        const IoWorkerCmdPack& workCmdPack, int createErrorAtIoNumber, QByteArray prefix) {
    QList<QByteArray> responseList;
    for(auto io : workCmdPack.m_workerIOList) {
        responseList.append(prefix + io.m_OutIn.m_bytesExpected);
    }
    if(createErrorAtIoNumber >= 0) {
        responseList[createErrorAtIoNumber] = "foo";
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

void IoWorker::setMaxPendingActions(int maxPackets)
{
    m_maxPendingCmdPacks = maxPackets;
}

int IoWorker::enqueueAction(IoWorkerCmdPack cmdPack)
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

bool IoWorker::isIoBusy()
{
    return m_currIoId.isActive();
}

void IoWorker::onIoFinished(int ioID, bool error)
{
    if(m_currIoId.isCurrAndDeactivateIf(ioID)) {
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
            m_currIoId.setCurrent(m_interface->sendAndReceive(
                        workerIo->m_OutIn.m_bytesSend,
                        &workerIo->m_dataReceived));
        }
    }
}

IoWorkerEntry* IoWorker::getNextIo()
{
    IoWorkerEntry* workerIo = nullptr;
    IoWorkerCmdPack *currCmdPack = getCurrentCmd();
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

void IoWorker::finishCurrentCmd()
{
    m_nextPosInWorkerIo = 0;
    finishCmd(m_pendingCmdPacks.takeFirst());
}

void IoWorker::finishCmd(IoWorkerCmdPack cmdToFinish)
{
    cmdToFinish.evalAll();
    emit sigCmdFinishedQueued(cmdToFinish);
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
        IoWorkerEntry& currentWorker = currCmdPack->m_workerIOList[m_nextPosInWorkerIo-1];
        IoSingleOutIn& currentOutIn = currentWorker.m_OutIn;
        currentWorker.m_IoEval = currentWorker.m_dataReceived == currentOutIn.m_bytesExpected ? IoWorkerEntry::EVAL_PASS : IoWorkerEntry::EVAL_FAIL;
        pass = currentWorker.m_IoEval == IoWorkerEntry::EVAL_PASS;
    }
    return pass;
}

bool IoWorker::canContinue()
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
