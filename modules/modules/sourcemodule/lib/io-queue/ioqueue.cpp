#include "ioqueue.h"

void IoQueue::setIoDevice(IoDeviceBase::Ptr ioDevice)
{
    if(m_ioDevice){
        disconnectIoDevice();
        m_ioDevice = nullptr;
    }
    if(ioDevice) {
        m_ioDevice = ioDevice;
        connectIoDevice();
    }
}

void IoQueue::setMaxPendingGroups(int maxGroups)
{
    m_maxPendingGroups = maxGroups;
}

void IoQueue::enqueueTransferGroup(IoQueueGroup::Ptr transferGroup)
{
    if(!canEnqueue(transferGroup)) {
        finishGroup(transferGroup);
    }
    else {
        m_pendingGroups.append(transferGroup);
        tryStartNextIo();
    }
}

bool IoQueue::isIoBusy() const
{
    return m_currIoId.hasPending();
}

void IoQueue::onIoFinished(int ioID, bool ioDeviceError)
{
    if(m_currIoId.isPendingAndRemoveIf(ioID)) {
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
    m_currIoId.clear();
    abortAllGroups();
}

void IoQueue::tryStartNextIo()
{
    if(!isIoBusy()) {
        IoTransferDataSingle::Ptr nextIo = getNextIoTransfer();
        if(nextIo) {
            int ioId = m_ioDevice->sendAndReceive(nextIo);
            m_currIoId.setPending(ioId);
        }
    }
}

IoTransferDataSingle::Ptr IoQueue::getNextIoTransfer()
{
    IoTransferDataSingle::Ptr nextIo;
    IoQueueGroup::Ptr currGroup = getCurrentGroup();
    if(currGroup) {
        if(m_nextPosInCurrGroup < currGroup->getTransferCount()) {
            nextIo = currGroup->getTransfer(m_nextPosInCurrGroup);
            m_nextPosInCurrGroup++;
        }
        else {
            if(!m_pendingGroups.isEmpty()) {
                finishCurrentGroup();
                nextIo = getNextIoTransfer();
            }
        }
    }
    return nextIo;
}

void IoQueue::connectIoDevice()
{
    connect(m_ioDevice.get(), &IoDeviceBase::sigIoFinished,
            this, &IoQueue::onIoFinished);
    connect(m_ioDevice.get(), &IoDeviceBase::sigDisconnected,
            this, &IoQueue::onIoDisconnected);
}

void IoQueue::disconnectIoDevice()
{
    disconnect(m_ioDevice.get(), &IoDeviceBase::sigIoFinished,
               this, &IoQueue::onIoFinished);
    disconnect(m_ioDevice.get(), &IoDeviceBase::sigDisconnected,
            this, &IoQueue::onIoDisconnected);
}

void IoQueue::finishCurrentGroup()
{
    m_nextPosInCurrGroup = 0;
    finishGroup(m_pendingGroups.takeFirst());
}

void IoQueue::finishGroup(IoQueueGroup::Ptr transferGroupToFinish)
{
    emit sigTransferGroupFinished(transferGroupToFinish);
}

void IoQueue::abortAllGroups()
{
    while(!m_pendingGroups.isEmpty()) {
        finishCurrentGroup();
    }
}

bool IoQueue::checkCurrentResponsePassed()
{
    bool pass = false;
    IoQueueGroup::Ptr currGroup = getCurrentGroup();
    if(currGroup) {
        IoTransferDataSingle::Ptr currentIo = currGroup->getTransfer(m_nextPosInCurrGroup-1);
        pass = currentIo->didIoPass();
    }
    return pass;
}

bool IoQueue::canEnqueue(IoQueueGroup::Ptr transferGroup)
{
    bool canEnqueue =
            m_ioDevice && m_ioDevice->isOpen() &&
            transferGroup && transferGroup->getTransferCount() > 0 &&
            (m_maxPendingGroups == 0 || m_pendingGroups.size() < m_maxPendingGroups);
    return canEnqueue;
}

bool IoQueue::canContinueCurrentGroup()
{
    bool pass = checkCurrentResponsePassed();
    IoQueueGroup::Ptr currGroup = getCurrentGroup();
    bool canContinue = false;
    if(currGroup) {
        if(!pass && currGroup->getErrorBehavior() == IoQueueErrorBehaviors::STOP_ON_FIRST_OK) {
            canContinue = true;
        }
        else if(pass && currGroup->getErrorBehavior() == IoQueueErrorBehaviors::STOP_ON_ERROR) {
            canContinue = true;
        }
        else if(currGroup->getErrorBehavior() == IoQueueErrorBehaviors::CONTINUE_ON_ERROR) {
            canContinue = true;
        }
    }
    return canContinue;
}

IoQueueGroup::Ptr IoQueue::getCurrentGroup()
{
    IoQueueGroup::Ptr current;
    if(!m_pendingGroups.isEmpty()) {
        current = m_pendingGroups.first();
    }
    return current;
}
