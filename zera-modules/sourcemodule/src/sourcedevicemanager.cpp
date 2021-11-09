#include "sourcedevicemanager.h"
#include "sourcedevice.h"
#include "sourceinterface.h"
#include "sourcescanner.h"
#include <random>

namespace SOURCEMODULE
{

cSourceDeviceManager::cSourceDeviceManager(int countSlots, QObject *parent) :
    QObject(parent),
    m_sourceDeviceSlots(QVector<cSourceDevice*>(countSlots, nullptr)),
    m_activeSlotCount(0),
    m_demoCount(0)
{
}

void cSourceDeviceManager::startSourceScan(const SourceInterfaceType interfaceType, const QString deviceInfo, const QUuid uuid)
{
    if(m_activeSlotCount >= m_sourceDeviceSlots.count()) {
        emit sigSourceScanFinished(-1, nullptr, uuid, QStringLiteral("No free slots"));
    }
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
                    this, &cSourceDeviceManager::onScanFinished,
                    Qt::QueuedConnection);
            connectTransaction->startScan();
        }
    }
    if(!started) {
        emit sigSourceScanFinished(-1, nullptr, uuid, QStringLiteral("Could not open Interface"));
    }
}


static bool randomBool() {
    // https://stackoverflow.com/questions/43329352/generating-random-boolean/43329456
    static auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    return gen();
}

QString randomString(int length) {
    QString strRnd;
    for(int cchar=0; cchar<length;cchar++) {
        strRnd += QString("%1").arg(randomBool());
    }
    return strRnd;
}

void cSourceDeviceManager::setDemoCount(int demoCount)
{
    int demoDiff = demoCount - m_demoCount;
    int maxSlots = m_sourceDeviceSlots.size();
    if(demoDiff > 0) {
        while(demoDiff && m_activeSlotCount < maxSlots) {
            startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
            demoDiff--;
        }
    }
    else if(demoDiff < 0) {
        demoDiff = -demoDiff;
        bool removeFront = randomBool();
        if(removeFront) {
            for(int slotNo=0; demoDiff && slotNo<maxSlots; slotNo++) {
                cSourceDevice* source = m_sourceDeviceSlots[slotNo];
                if(source) {
                    if(source->isDemo()) {
                        static_cast<cSourceInterfaceDemo*>(source->ioInterface().get())->simulateExternalDisconnect();
                        demoDiff--;
                    }
                }
            }
        }
        else {
            for(int slotNo=maxSlots-1; demoDiff && slotNo>=0; slotNo--) {
                cSourceDevice* source = m_sourceDeviceSlots[slotNo];
                if(source) {
                    if(source->isDemo()) {
                        static_cast<cSourceInterfaceDemo*>(source->ioInterface().get())->simulateExternalDisconnect();
                        demoDiff--;
                    }
                }
            }
        }
    }
}

bool cSourceDeviceManager::removeSource(int slotNo)
{
    bool removed = false;
    auto &sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
    if(sourceDeviceCurr) {
        m_activeSlotCount--;
        if(sourceDeviceCurr->isDemo()) {
            m_demoCount--;
        }
        disconnect(sourceDeviceCurr, &cSourceDevice::sigClosed, this, &cSourceDeviceManager::onRemoveSource);
        delete sourceDeviceCurr;
        sourceDeviceCurr = nullptr;
        emit sigSlotRemoved(slotNo);
        removed = true;
    }
    return removed;
}


void cSourceDeviceManager::onScanFinished(QSharedPointer<cSourceScanner> transaction)
{
    disconnect(transaction.get(), &cSourceScanner::sigTransactionFinished, this, &cSourceDeviceManager::onScanFinished);
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
                if(sourceDeviceCurr->isDemo()) {
                    m_demoCount++;
                }
                connect(sourceDeviceFound, &cSourceDevice::sigClosed, this, &cSourceDeviceManager::onRemoveSource);
                emit sigSourceScanFinished(slotNo, sourceDeviceFound, transaction->getUuid(), QString());
                break;
            }
        }
    }
    // cleanup if something went wrong
    if(!slotAdded) {
        QString erorDesc;
        // although we check at entry startSourceScan we have to recheck here
        // because startSourceScan cannot update m_activeSlotCount - scan can fail
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

int cSourceDeviceManager::demoCount()
{
    return m_demoCount;
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
            removeSource(slotNo);
            break;
        }
    }
}


}
