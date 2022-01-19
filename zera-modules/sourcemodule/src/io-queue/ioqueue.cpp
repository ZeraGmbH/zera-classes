#include "ioqueue.h"

IoQueue::IoQueue(QObject *parent) : QObject(parent)
{
    connect(this, &IoQueue::sigTransferGroupFinishedQueued,
            this, &IoQueue::sigTransferGroupFinished, Qt::QueuedConnection);
}

void IoQueue::setIoInterface(tIoDeviceShPtr interface)
{
    m_interface = interface;
    if(interface) {
        connect(m_interface.get(), &IODeviceBaseSerial::sigIoFinished,
                this, &IoQueue::onIoFinished);
        connect(m_interface.get(), &IODeviceBaseSerial::sigDisconnected,
                this, &IoQueue::onIoDisconnected);
    }
    else if(m_interface){
        disconnect(m_interface.get(), &IODeviceBaseSerial::sigIoFinished,
                   this, &IoQueue::onIoFinished);
        disconnect(m_interface.get(), &IODeviceBaseSerial::sigDisconnected,
                this, &IoQueue::onIoDisconnected);
        m_interface = nullptr;
    }
}

void IoQueue::setMaxPendingGroups(int maxGroups)
{
    m_maxPendingGroups = maxGroups;
}

int IoQueue::enqueueTransferGroup(IoTransferDataGroup transferGroup)
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

bool IoQueue::isIoBusy() const
{
    return m_currIoId.isActive();
}

void IoQueue::onIoFinished(int ioID, bool ioDeviceError)
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

void IoQueue::onIoDisconnected()
{
    m_currIoId.deactivate();
    abortAllGroups();
    setIoInterface(nullptr);
}

void IoQueue::tryStartNextIo()
{
    if(!isIoBusy()) {
        tIoTransferDataSingleShPtr nextIo = getNextIoTransfer();
        if(nextIo) {
            m_interface->setReadTimeoutNextIo(nextIo->m_responseTimeoutMs);
            int ioId = m_interface->sendAndReceive(nextIo);
            m_currIoId.setCurrent(ioId);
        }
    }
}

tIoTransferDataSingleShPtr IoQueue::getNextIoTransfer()
{
    tIoTransferDataSingleShPtr nextIo;
    IoTransferDataGroup *currGroup = getCurrentGroup();
    if(currGroup) {
        if(m_nextPosInCurrGroup < currGroup->m_ioTransferList.count()) {
            nextIo = currGroup->m_ioTransferList[m_nextPosInCurrGroup];
            m_nextPosInCurrGroup++;
        }
        else {
            finishCurrentGroup();
            nextIo = getNextIoTransfer();
        }
    }
    return nextIo;
}

void IoQueue::finishCurrentGroup()
{
    m_nextPosInCurrGroup = 0;
    finishGroup(m_pendingGroups.takeFirst());
}

void IoQueue::finishGroup(IoTransferDataGroup transferGroupToFinish)
{
    transferGroupToFinish.evalAll();
    emit sigTransferGroupFinishedQueued(transferGroupToFinish);
}

void IoQueue::abortAllGroups()
{
    while(!m_pendingGroups.isEmpty()) {
        finishCurrentGroup();
    }
}

bool IoQueue::evaluateResponse()
{
    bool pass = false;
    IoTransferDataGroup *currGroup = getCurrentGroup();
    if(currGroup) {
        tIoTransferDataSingleShPtr currentIo = currGroup->m_ioTransferList[m_nextPosInCurrGroup-1];
        pass = currentIo->evaluateResponse();
    }
    return pass;
}

bool IoQueue::canEnqueue(IoTransferDataGroup transferGroup)
{
    bool canEnqueue =
            m_interface && m_interface->isOpen() &&
            !transferGroup.m_ioTransferList.isEmpty() &&
            (m_maxPendingGroups == 0 || m_pendingGroups.size() < m_maxPendingGroups);
    return canEnqueue;
}

bool IoQueue::canContinueCurrentGroup()
{
    bool pass = evaluateResponse();
    IoTransferDataGroup *currGroup = getCurrentGroup();
    return pass || (currGroup && currGroup->m_errorBehavior == BEHAVE_CONTINUE_ON_ERROR);
}

IoTransferDataGroup *IoQueue::getCurrentGroup()
{
    IoTransferDataGroup* current = nullptr;
    if(!m_pendingGroups.isEmpty()) {
        current = &m_pendingGroups.first();
    }
    return current;
}
