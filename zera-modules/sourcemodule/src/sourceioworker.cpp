#include "sourceioworker.h"

cSourceIoWorker::cSourceIoWorker(QObject *parent) : QObject(parent)
{
    connect(this, &cSourceIoWorker::workPackFinishedQueued,
            this, &cSourceIoWorker::workPackFinished, Qt::QueuedConnection);
}

void cSourceIoWorker::setIoInterface(QSharedPointer<cSourceInterfaceBase> interface)
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

cWorkerPacket cSourceIoWorker::commandPackToWorkerPack(const cSourceCommandPacket &commandPack)
{
    cWorkerPacket workPack;
    workPack.m_commandType = commandPack.m_commandType;
    for(auto outIn : commandPack.m_singleOutInList) {
        cSourceIoWorkerEntry workEntry;
        workEntry.m_OutIn = outIn;
        workPack.m_workerList.append(workEntry);
    }
    return workPack;
}

int cSourceIoWorker::enqueueIoPacket(cWorkerPacket workPack)
{
    if(!m_interface || workPack.m_workerList.isEmpty()) {
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
    return m_currentWorkPack.m_workerId != 0;
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
            cSourceSingleOutIn outIn = getNextOutIn();
            if(cSourceActionTypes::isValidType(outIn.m_actionType)) {

            }
        }
        else {

        }
    }
}

cSourceSingleOutIn cSourceIoWorker::getNextOutIn()
{
    cSourceSingleOutIn outIn;

    return outIn;
}

