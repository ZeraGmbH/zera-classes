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
        return;
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

void cSourceDeviceManager::setDemoCount(int getDemoCount)
{
    int demoDiff = getDemoCount - m_demoCount;
    if(demoDiff > 0) {
        while(demoDiff && m_activeSlotCount < getSlotCount()) {
            startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
            demoDiff--;
        }
    }
    else if(demoDiff < 0) {
        demoDiff = -demoDiff;
        bool removeFront = randomBool();
        if(removeFront) {
            for(int slotNo=0; demoDiff && slotNo<getSlotCount(); slotNo++) {
                cSourceDevice* source = m_sourceDeviceSlots[slotNo];
                if(source) {
                    if(source->isDemo()) {
                        static_cast<cSourceInterfaceDemo*>(source->getIoInterface().get())->simulateExternalDisconnect();
                        demoDiff--;
                    }
                }
            }
        }
        else {
            for(int slotNo=getSlotCount()-1; demoDiff && slotNo>=0; slotNo--) {
                cSourceDevice* source = m_sourceDeviceSlots[slotNo];
                if(source) {
                    if(source->isDemo()) {
                        static_cast<cSourceInterfaceDemo*>(source->getIoInterface().get())->simulateExternalDisconnect();
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

int cSourceDeviceManager::getSlotCount()
{
    return m_sourceDeviceSlots.size();
}

void cSourceDeviceManager::onScanFinished(QSharedPointer<cSourceScanner> transaction)
{
    disconnect(transaction.get(), &cSourceScanner::sigTransactionFinished, this, &cSourceDeviceManager::onScanFinished);
    cSourceDevice *getSourceDeviceFound = transaction->getSourceDeviceFound();
    // add to first free slot
    bool slotAdded = false;
    if(getSourceDeviceFound && m_activeSlotCount < m_sourceDeviceSlots.count()) {
        // find free slot
        for(int slotNo=0; slotNo<m_sourceDeviceSlots.count(); slotNo++) {
            auto &sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
            if(sourceDeviceCurr == nullptr) {
                slotAdded = true;
                sourceDeviceCurr = getSourceDeviceFound;
                m_activeSlotCount++;
                if(sourceDeviceCurr->isDemo()) {
                    m_demoCount++;
                }
                connect(getSourceDeviceFound, &cSourceDevice::sigClosed, this, &cSourceDeviceManager::onRemoveSource);
                emit sigSourceScanFinished(slotNo, getSourceDeviceFound, transaction->getUuid(), QString());
                break;
            }
        }
    }
    // cleanup if something went wrong
    if(!slotAdded) {
        QString erorDesc;
        // although we check at entry startSourceScan we have to recheck here
        // because startSourceScan cannot update m_activeSlotCount - scan can fail
        if(getSourceDeviceFound) {
            delete getSourceDeviceFound;
            getSourceDeviceFound = nullptr;
            erorDesc = QStringLiteral("Slots full");
        }
        else {
            erorDesc = QStringLiteral("No source device found");
        }
        // we need to notify failures
        emit sigSourceScanFinished(-1, getSourceDeviceFound, transaction->getUuid(), erorDesc);
    }
}


int cSourceDeviceManager::getActiveSlotCount()
{
    return m_activeSlotCount;
}

int cSourceDeviceManager::getDemoCount()
{
    return m_demoCount;
}


cSourceDevice *cSourceDeviceManager::getSourceDevice(int slotNo)
{
    cSourceDevice* getSourceDevice = nullptr;
    if(slotNo < m_sourceDeviceSlots.count()) {
        getSourceDevice = m_sourceDeviceSlots.at(slotNo);
    }
    return getSourceDevice;
}


void cSourceDeviceManager::onRemoveSource(cSourceDevice *getSourceDevice)
{
    for(int slotNo=0; slotNo<m_sourceDeviceSlots.count(); slotNo++) {
        auto &sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
        if(sourceDeviceCurr && sourceDeviceCurr == getSourceDevice) {
            removeSource(slotNo);
            break;
        }
    }
}


}
