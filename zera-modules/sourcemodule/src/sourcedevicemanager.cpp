#include "sourcedevicemanager.h"
#include "sourcedevice.h"
#include "sourceinterface.h"
#include "sourcescanner.h"

namespace SOURCEMODULE
{

cSourceDeviceManager::cSourceDeviceManager(int countSlots, QObject *parent) :
    QObject(parent),
    m_sourceDeviceSlots(QVector<cSourceDevice*>(countSlots, nullptr)),
    m_activeSlotCount(0)
{
}

void cSourceDeviceManager::startSourceScan(const SourceInterfaceType interfaceType, const QString deviceInfo, const QUuid uuid)
{
    bool started = false;
    QSharedPointer<cSourceInterfaceBase> interface =
            QSharedPointer<cSourceInterfaceBase>(cSourceInterfaceFactory::createSourceInterface(SourceInterfaceType(interfaceType)));
    if(interface) {
        started = interface->open(deviceInfo);
        if(started) {
            QSharedPointer<cSourceScanner> connectTransaction = QSharedPointer<cSourceScanner>(new cSourceScanner(interface, uuid));
            // in case our module=we are killed while scan is pending we have to make sure
            // that scanner finishes and deletes itself after completion (our shared reference is gone)
            connectTransaction->setScannerReference(connectTransaction);
            connect(connectTransaction.get(), &cSourceScanner::sigTransactionFinished,
                    this, &cSourceDeviceManager::onIdentificationTransactionFinished,
                    Qt::QueuedConnection);
            connectTransaction->startScan();
        }
    }
    if(!started) {
        emit sigSourceScanFinished(-1, nullptr, uuid, QStringLiteral("Could not open Interface"));
    }
}


void cSourceDeviceManager::onIdentificationTransactionFinished(QSharedPointer<cSourceScanner> transaction)
{
    disconnect(transaction.get(), &cSourceScanner::sigTransactionFinished, this, &cSourceDeviceManager::onIdentificationTransactionFinished);
    cSourceDevice *sourceDeviceFound = transaction->sourceDeviceFound();
    // add to first free slot
    bool slotAdded = false;
    if(sourceDeviceFound && m_activeSlotCount < m_sourceDeviceSlots.count()) {
        // find free slot
        for(int slotNo=0; slotNo<m_sourceDeviceSlots.count(); slotNo++) {
            auto &sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
            if(sourceDeviceCurr == nullptr) {
                slotAdded = true;
                sourceDeviceCurr = sourceDeviceFound;
                m_activeSlotCount++;
                connect(sourceDeviceFound, &cSourceDevice::sigClosed, this, &cSourceDeviceManager::onRemoveSource);
                emit sigSourceScanFinished(slotNo, sourceDeviceFound, transaction->getUuid(), QString());
                break;
            }
        }
    }
    // cleanup if something went wrong
    if(!slotAdded) {
        QString erorDesc;
        if(sourceDeviceFound) {
            delete sourceDeviceFound;
            sourceDeviceFound = nullptr;
            erorDesc = QStringLiteral("Slots full");
        }
        else {
            erorDesc = QStringLiteral("No source device found");
        }
        // we need to notify failures
        emit sigSourceScanFinished(-1, sourceDeviceFound, transaction->getUuid(), erorDesc);
    }
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
