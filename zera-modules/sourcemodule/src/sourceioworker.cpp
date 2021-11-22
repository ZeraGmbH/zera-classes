#include "sourceioworker.h"

bool cSourceIoWorkerEntry::operator ==(const cSourceIoWorkerEntry &other)
{
    // this for purpose of test -> ignore m_dataReceived / m_IoEval
    return m_OutIn == other.m_OutIn;
}


bool cWorkerCommandPacket::operator ==(const cWorkerCommandPacket &other)
{
    return  m_workerId == other.m_workerId &&
            m_commandType == other.m_commandType &&
            m_errorBehavior == other.m_errorBehavior &&
            m_workerIOList == other.m_workerIOList;
}


cWorkerCommandPacket cSourceWorkerConverter::commandPackToWorkerPack(const cSourceCommandPacket &commandPack)
{
    cWorkerCommandPacket workPack;
    workPack.m_commandType = commandPack.m_commandType;
    workPack.m_errorBehavior = commandPack.m_errorBehavior;
    for(auto outIn : commandPack.m_singleOutInList) {
        cSourceIoWorkerEntry workEntry;
        workEntry.m_OutIn = outIn;
        workPack.m_workerIOList.append(workEntry);
    }
    return workPack;
}


cSourceIoWorker::cSourceIoWorker(QObject *parent) : QObject(parent)
{
    connect(this, &cSourceIoWorker::sigPackFinishedQueued,
            this, &cSourceIoWorker::sigPackFinished, Qt::QueuedConnection);
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

int cSourceIoWorker::enqueuePacket(cWorkerCommandPacket workPack)
{
    if(!m_interface || workPack.m_workerIOList.isEmpty()) {
        emit sigPackFinishedQueued(workPack);
    }
    else {
        workPack.m_workerId = m_IdGenerator.nextID();
        m_pendingWorkPacks.append(workPack);
        tryStartNextIo();
    }
    return workPack.m_workerId;
}

bool cSourceIoWorker::isBusy()
{
    return m_iCurrentIoID != 0;
}

void cSourceIoWorker::onIoFinished(int ioID)
{
    if(ioID == 0) {
        abortAllWorkers();
    }
    else {
        m_iCurrentIoID = 0;
        if(canContinue()) {
            tryStartNextIo();
        }
        else {
            abortAllWorkers();
        }
    }
}

void cSourceIoWorker::onIoDisconnected()
{
    abortAllWorkers();
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
    cWorkerCommandPacket *currCmdPack = getCurrentCmdPack();
    if(currCmdPack) {
        if(m_nextPosInWorkerIo < currCmdPack->m_workerIOList.count()) {
            workerIo = &(currCmdPack->m_workerIOList[m_nextPosInWorkerIo]);
            m_nextPosInWorkerIo++;
        }
        else {
            finishCurrentWorker();
            workerIo = getNextIo();
        }
    }
    return workerIo;
}

void cSourceIoWorker::finishCurrentWorker()
{
    m_nextPosInWorkerIo = 0;
    m_iCurrentIoID = 0;
    cWorkerCommandPacket finishedPack = m_pendingWorkPacks.takeFirst();
    emit sigPackFinished(finishedPack);
}

void cSourceIoWorker::abortAllWorkers()
{
    while(!m_pendingWorkPacks.isEmpty()) {
        finishCurrentWorker();
    }
}

bool cSourceIoWorker::evaluateResponse()
{
    bool pass = false;
    cWorkerCommandPacket *currCmdPack = getCurrentCmdPack();
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
    cWorkerCommandPacket *currCmdPack = getCurrentCmdPack();
    return pass || (currCmdPack && currCmdPack->m_errorBehavior == BEHAVE_CONTINUE_ON_ERROR);
}

cWorkerCommandPacket *cSourceIoWorker::getCurrentCmdPack()
{
    cWorkerCommandPacket* current = nullptr;
    if(!m_pendingWorkPacks.isEmpty()) {
        current = &m_pendingWorkPacks.first();
    }
    return current;
}
