#include "ioworker.h"

IoWorker::IoWorker(QObject *parent) : QObject(parent)
{
    connect(this, &IoWorker::sigTransferGroupFinishedQueued,
            this, &IoWorker::sigTransferGroupFinished, Qt::QueuedConnection);
}

void IoWorker::setIoInterface(tIoDeviceShPtr interface)
{
    m_interface = interface;
    if(interface) {
        connect(m_interface.get(), &IODeviceBaseSerial::sigIoFinished,
                this, &IoWorker::onIoFinished);
        connect(m_interface.get(), &IODeviceBaseSerial::sigDisconnected,
                this, &IoWorker::onIoDisconnected);
    }
    else if(m_interface){
        disconnect(m_interface.get(), &IODeviceBaseSerial::sigIoFinished,
                   this, &IoWorker::onIoFinished);
        disconnect(m_interface.get(), &IODeviceBaseSerial::sigDisconnected,
                this, &IoWorker::onIoDisconnected);
        m_interface = nullptr;
    }
}

void IoWorker::setMaxPendingGroups(int maxGroups)
{
    m_maxPendingGroups = maxGroups;
}

int IoWorker::enqueueTransferGroup(IoTransferDataGroup transferGroup)
{
    if(!canEnqueue(transferGroup)) {
        finishGroup(transferGroup);
    }
    else {
        transferGroup.m_groupId = m_IdGenerator.nextID();
        m_pendingGroups.append(transferGroup);
        tryStartNextIo();
    }
    return transferGroup.m_groupId;
}

bool IoWorker::isIoBusy() const
{
    return m_currIoId.isActive();
}

void IoWorker::onIoFinished(int ioID, bool ioDeviceError)
{
    if(m_currIoId.isCurrAndDeactivateIf(ioID)) {
        if(ioDeviceError) {
            abortAllGroups();
        }
        else {
            if(!canContinueCurrentGroup()) {
                finishCurrentGroup();
            }
            tryStartNextIo();
        }
    }
}

void IoWorker::onIoDisconnected()
{
    m_currIoId.deactivate();
    abortAllGroups();
    setIoInterface(nullptr);
}

void IoWorker::tryStartNextIo()
{
    if(!isIoBusy()) {
        tIoTransferDataSingleShPtr workerIo = getNextIoTransfer();
        if(workerIo) {
            m_interface->setReadTimeoutNextIo(workerIo->m_responseTimeoutMs);
            int ioId = m_interface->sendAndReceive(workerIo);
            m_currIoId.setCurrent(ioId);
        }
    }
}

tIoTransferDataSingleShPtr IoWorker::getNextIoTransfer()
{
    tIoTransferDataSingleShPtr workerIo;
    IoTransferDataGroup *currGroup = getCurrentGroup();
    if(currGroup) {
        if(m_nextPosInCurrGroup < currGroup->m_ioTransferList.count()) {
            workerIo = currGroup->m_ioTransferList[m_nextPosInCurrGroup];
            m_nextPosInCurrGroup++;
        }
        else {
            finishCurrentGroup();
            workerIo = getNextIoTransfer();
        }
    }
    return workerIo;
}

void IoWorker::finishCurrentGroup()
{
    m_nextPosInCurrGroup = 0;
    finishGroup(m_pendingGroups.takeFirst());
}

void IoWorker::finishGroup(IoTransferDataGroup transferGroupToFinish)
{
    transferGroupToFinish.evalAll();
    emit sigTransferGroupFinishedQueued(transferGroupToFinish);
}

void IoWorker::abortAllGroups()
{
    while(!m_pendingGroups.isEmpty()) {
        finishCurrentGroup();
    }
}

bool IoWorker::evaluateResponse()
{
    bool pass = false;
    IoTransferDataGroup *currGroup = getCurrentGroup();
    if(currGroup) {
        tIoTransferDataSingleShPtr currentIo = currGroup->m_ioTransferList[m_nextPosInCurrGroup-1];
        pass = currentIo->evaluateResponse();
    }
    return pass;
}

bool IoWorker::canEnqueue(IoTransferDataGroup transferGroup)
{
    bool canEnqueue =
            m_interface && m_interface->isOpen() &&
            !transferGroup.m_ioTransferList.isEmpty() &&
            (m_maxPendingGroups == 0 || m_pendingGroups.size() < m_maxPendingGroups);
    return canEnqueue;
}

bool IoWorker::canContinueCurrentGroup()
{
    bool pass = evaluateResponse();
    IoTransferDataGroup *currGroup = getCurrentGroup();
    return pass || (currGroup && currGroup->m_errorBehavior == BEHAVE_CONTINUE_ON_ERROR);
}

IoTransferDataGroup *IoWorker::getCurrentGroup()
{
    IoTransferDataGroup* current = nullptr;
    if(!m_pendingGroups.isEmpty()) {
        current = &m_pendingGroups.first();
    }
    return current;
}
