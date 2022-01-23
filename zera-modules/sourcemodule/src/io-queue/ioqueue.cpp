#include "ioqueue.h"

IoQueue::IoQueue(QObject *parent) : QObject(parent)
{
    connect(this, &IoQueue::sigTransferGroupFinishedQueued,
            this, &IoQueue::sigTransferGroupFinished, Qt::QueuedConnection);
}

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

int IoQueue::enqueueTransferGroup(IoTransferDataGroup transferGroup)
{
    if(!canEnqueue(transferGroup)) {
        finishGroup(transferGroup);
    }
    else {
        m_pendingGroups.append(transferGroup);
        tryStartNextIo();
    }
    return transferGroup.getGroupId();
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
    setIoDevice(nullptr);
}

void IoQueue::tryStartNextIo()
{
    if(!isIoBusy()) {
        IoTransferDataSingle::Ptr nextIo = getNextIoTransfer();
        if(nextIo) {
            int ioId = m_ioDevice->sendAndReceive(nextIo);
            m_currIoId.setCurrent(ioId);
        }
    }
}

IoTransferDataSingle::Ptr IoQueue::getNextIoTransfer()
{
    IoTransferDataSingle::Ptr nextIo;
    IoTransferDataGroup *currGroup = getCurrentGroup();
    if(currGroup) {
        if(m_nextPosInCurrGroup < currGroup->getTransferCount()) {
            nextIo = currGroup->getTransfer(m_nextPosInCurrGroup);
            m_nextPosInCurrGroup++;
        }
        else {
            finishCurrentGroup();
            nextIo = getNextIoTransfer();
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

bool IoQueue::checkCurrentResponsePassed()
{
    bool pass = false;
    IoTransferDataGroup *currGroup = getCurrentGroup();
    if(currGroup) {
        IoTransferDataSingle::Ptr currentIo = currGroup->getTransfer(m_nextPosInCurrGroup-1);
        pass = currentIo->didIoPass();
    }
    return pass;
}

bool IoQueue::canEnqueue(IoTransferDataGroup transferGroup)
{
    bool canEnqueue =
            m_ioDevice && m_ioDevice->isOpen() &&
            transferGroup.getTransferCount() > 0 &&
            (m_maxPendingGroups == 0 || m_pendingGroups.size() < m_maxPendingGroups);
    return canEnqueue;
}

bool IoQueue::canContinueCurrentGroup()
{
    bool pass = checkCurrentResponsePassed();
    IoTransferDataGroup *currGroup = getCurrentGroup();
    return pass || (currGroup && currGroup->getErrorBehavior() == IoTransferDataGroup::BEHAVE_CONTINUE_ON_ERROR);
}

IoTransferDataGroup *IoQueue::getCurrentGroup()
{
    IoTransferDataGroup* current = nullptr;
    if(!m_pendingGroups.isEmpty()) {
        current = &m_pendingGroups.first();
    }
    return current;
}
