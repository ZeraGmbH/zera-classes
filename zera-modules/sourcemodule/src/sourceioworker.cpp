#include "sourceioworker.h"

cSourceIoWorker::cSourceIoWorker(QObject *parent) : QObject(parent)
{
    connect(this, &cSourceIoWorker::workPackFinishedQueued,
            this, &cSourceIoWorker::workPackFinished, Qt::QueuedConnection);
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
        emit workPackFinishedQueued(workPack);
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

}

void cSourceIoWorker::onIoDisconnected()
{
    onIoFinished(m_iCurrentIoID);
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
            m_iPositionInWorkerIo = 0;
            cWorkerCommandPacket finishedPack = m_pendingWorkPacks.takeFirst();
            emit workPackFinished(finishedPack);
            workerIo = getNextWorkerIO();
        }
    }
    return workerIo;
}

