#include "sourcedevicemanager.h"
#include "sourcedevice.h"
#include "sourceinterface.h"
#include "sourceconnecttransaction.h"

namespace SOURCEMODULE
{

cSourceDeviceManager::cSourceDeviceManager(int countSlots, QObject *parent) :
    QObject(parent),
    m_sourceDeviceSlots(QVector<cSourceDevice*>(countSlots, nullptr)),
    m_activeSlotCount(0)
{
}

void cSourceDeviceManager::startSourceScan(cSourceInterfaceBase* interface)
{
    cSourceConnectTransaction* connectTransaction = new cSourceConnectTransaction(interface);
    connect(connectTransaction, &cSourceConnectTransaction::sigTransactionFinished, this, &cSourceDeviceManager::onIdentificationTransactionFinished);
    connectTransaction->startScan();
}


void cSourceDeviceManager::onIdentificationTransactionFinished(cSourceConnectTransaction *transaction)
{
    disconnect(transaction, &cSourceConnectTransaction::sigTransactionFinished, this, &cSourceDeviceManager::onIdentificationTransactionFinished);
    cSourceDevice *sourceDeviceFound = transaction->sourceDeviceFound();
    if(sourceDeviceFound) {
        addSource(sourceDeviceFound);
    }
    delete transaction;
}


int cSourceDeviceManager::activeSlotCount()
{
    return m_activeSlotCount;
}


cSourceDevice *cSourceDeviceManager::sourceDevice(int slotNo)
{
    cSourceDevice* sourceDevice = nullptr;
    if(slotNo < m_sourceDeviceSlots.count()) {
        sourceDevice = m_sourceDeviceSlots.at(slotNo);
    }
    return sourceDevice;
}


void cSourceDeviceManager::addSource(cSourceDevice *sourceDevice)
{
    bool slotAdded = false;
    if(m_activeSlotCount < m_sourceDeviceSlots.count()) {
        // find free slot
        for(int slotNo=0; slotNo<m_sourceDeviceSlots.count(); slotNo++) {
            auto &sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
            if(sourceDeviceCurr == nullptr) {
                slotAdded = true;
                sourceDeviceCurr = sourceDevice;
                m_activeSlotCount++;
                connect(sourceDevice, &cSourceDevice::sigClosed, this, &cSourceDeviceManager::onRemoveSource);
                emit sigSlotAdded(slotNo, sourceDevice);
                break;
            }
        }
    }
    if(!slotAdded) {
        qWarning("Could not add source device to source device manager!");
    }
}


void cSourceDeviceManager::onRemoveSource(cSourceDevice *sourceDevice)
{
    for(int slotNo=0; slotNo<m_sourceDeviceSlots.count(); slotNo++) {
        auto &sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
        if(sourceDeviceCurr && sourceDeviceCurr == sourceDevice) {
            m_activeSlotCount--;
            disconnect(sourceDevice, &cSourceDevice::sigClosed, this, &cSourceDeviceManager::onRemoveSource);
            delete sourceDeviceCurr;
            sourceDeviceCurr = nullptr;
            emit sigSlotRemoved(slotNo);
            break;
        }
    }
}




}
