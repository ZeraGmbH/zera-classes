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

cSourceIoWorker::cSourceIoWorker(QObject *parent) : QObject(parent)
{
    connect(this, &cSourceIoWorker::sigWorkPackFinishedQueued,
            this, &cSourceIoWorker::sigWorkPackFinished, Qt::QueuedConnection);
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

cWorkerCommandPacket cSourceIoWorker::commandPackToWorkerPack(const cSourceCommandPacket &commandPack)
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

int cSourceIoWorker::enqueueIoPacket(cWorkerCommandPacket workPack)
{
    if(!m_interface || workPack.m_workerIOList.isEmpty()) {
        emit sigWorkPackFinishedQueued(workPack);
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
        finishCurrentWorker();
    }
}

void cSourceIoWorker::onIoDisconnected()
{
    if(!m_pendingWorkPacks.isEmpty()) { // for disconnect before first enqueue
        onIoFinished(m_iCurrentIoID);
    }
    setIoInterface(nullptr);
}

void cSourceIoWorker::tryStartNextIo()
{
    if(!isBusy()) {
        if(m_interface) {
            cSourceIoWorkerEntry* workerIo = getNextWorkerIO();
            if(workerIo) {
                m_iCurrentIoID = m_interface->sendAndReceive(
                            workerIo->m_OutIn.m_bytesSend,
                            &workerIo->m_dataReceived);
            }
        }
        else {

        }
    }
}

cSourceIoWorkerEntry* cSourceIoWorker::getNextWorkerIO()
{
    cSourceIoWorkerEntry* workerIo = nullptr;
    if(!m_pendingWorkPacks.isEmpty()) {
        cWorkerCommandPacket &currentPack = m_pendingWorkPacks.first();
        if(m_iPositionInWorkerIo < currentPack.m_workerIOList.count()) {
            workerIo = &(currentPack.m_workerIOList[m_iPositionInWorkerIo]);
            m_iPositionInWorkerIo++;
        }
        else {
            finishCurrentWorker();
            workerIo = getNextWorkerIO();
        }
    }
    return workerIo;
}

void cSourceIoWorker::finishCurrentWorker()
{
    m_iPositionInWorkerIo = 0;
    cWorkerCommandPacket finishedPack = m_pendingWorkPacks.takeFirst();
    emit sigWorkPackFinished(finishedPack);
}
