#include "sourcedevicemanager.h"
#include "sourcedevice.h"
#include "sourceinterface.h"
#include "sourcescanner.h"
#include <random>

// DEMO helper
static bool randomBool() {
    // https://stackoverflow.com/questions/43329352/generating-random-boolean/43329456
    static auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    return gen();
}

namespace SOURCEMODULE
{

cSourceDeviceManager::cSourceDeviceManager(int countSlots, QObject *parent) :
    QObject(parent),
    m_sourceDeviceSlots(QVector<cSourceDevice*>(countSlots, nullptr))
{
    connect(this, &cSourceDeviceManager::sigSlotRemovedQueued,
            this, &cSourceDeviceManager::sigSlotRemoved, Qt::QueuedConnection);
}

void cSourceDeviceManager::startSourceScan(const SourceInterfaceTypes interfaceType, const QString deviceInfo, const QUuid uuid)
{
    if(m_activeSlotCount >= m_sourceDeviceSlots.count()) {
        emit sigSourceScanFinished(-1, nullptr, uuid, QStringLiteral("No free slots"));
        return;
    }
    bool started = false;
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SourceInterfaceTypes(interfaceType));
    if(interface) {
        started = interface->open(deviceInfo);
        if(started) {
            tSourceScannerShPtr sourceScanner = cSourceScanner::createScanner(interface, uuid);
            connect(sourceScanner.get(), &cSourceScanner::sigScanFinished,
                    this, &cSourceDeviceManager::onScanFinished,
                    Qt::QueuedConnection);
            sourceScanner->startScan();
        }
    }
    if(!started) {
        emit sigSourceScanFinished(-1, nullptr, uuid, QStringLiteral("Could not open Interface"));
    }
}

void cSourceDeviceManager::setDemoCount(int count)
{
    int demoDiff = count - m_demoCount;
    if(demoDiff > 0) { // add
        while(demoDiff && m_activeSlotCount < getSlotCount()) {
            startSourceScan(SOURCE_INTERFACE_DEMO, "Demo", QUuid::createUuid());
            demoDiff--;
        }
    }
    else if(demoDiff < 0) { // remove
        demoDiff = -demoDiff;
        bool removeFront = randomBool();
        if(removeFront) {
            for(int slotNo=0; demoDiff && slotNo<getSlotCount(); slotNo++) {
                if(tryStartDemoDeviceRemove(slotNo)) {
                    demoDiff--;
                }
            }
        }
        else {
            for(int slotNo=getSlotCount()-1; demoDiff && slotNo>=0; slotNo--) {
                if(tryStartDemoDeviceRemove(slotNo)) {
                    demoDiff--;
                }
            }
        }
    }
}

bool cSourceDeviceManager::closeSource(int slotNo)
{
    bool closeRequested = false;
    if(isValidSlotNo(slotNo)) {
        auto sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
        if(sourceDeviceCurr) {
            sourceDeviceCurr->close();
            closeRequested = true;
        }
    }
    return closeRequested;
}

int cSourceDeviceManager::getSlotCount()
{
    return m_sourceDeviceSlots.size();
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
    if(isValidSlotNo(slotNo)) {
        getSourceDevice = m_sourceDeviceSlots.at(slotNo);
    }
    return getSourceDevice;
}

void cSourceDeviceManager::onScanFinished(tSourceScannerShPtr scanner)
{
    disconnect(scanner.get(), &cSourceScanner::sigScanFinished, this, &cSourceDeviceManager::onScanFinished);

    cSourceDevice *sourceDeviceFound = scanner->getSourceDeviceFound();
    QString erorDesc;
    int freeSlot = findFreeSlot();
    if(sourceDeviceFound) {
        if(freeSlot >= 0) {
            addSource(freeSlot, sourceDeviceFound);
        }
        else {
            delete sourceDeviceFound;
            sourceDeviceFound = nullptr;
            erorDesc = QStringLiteral("Slots full");
        }
    }
    else {
        erorDesc = QStringLiteral("No source device found");
    }
    emit sigSourceScanFinished(freeSlot, sourceDeviceFound, scanner->getUuid(), erorDesc);
}

void cSourceDeviceManager::onSourceClosed(cSourceDevice *sourceDevice)
{
    for(int slotNo=0; slotNo<m_sourceDeviceSlots.count(); slotNo++) {
        auto &sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
        if(sourceDeviceCurr && sourceDeviceCurr == sourceDevice) {
            m_activeSlotCount--;
            if(sourceDeviceCurr->isDemo()) {
                m_demoCount--;
            }
            disconnect(sourceDeviceCurr, &cSourceDevice::sigClosed, this, &cSourceDeviceManager::onSourceClosed);
            delete sourceDeviceCurr;
            sourceDeviceCurr = nullptr;
            emit sigSlotRemovedQueued(slotNo);
            break;
        }
    }
}

bool cSourceDeviceManager::isValidSlotNo(int slotNo)
{
    return slotNo >= 0 && slotNo<m_sourceDeviceSlots.count();
}

int cSourceDeviceManager::findFreeSlot()
{
    int freeSlotNo = -1;
    if(m_activeSlotCount < getSlotCount()) {
        for(int slotNo=0; slotNo<getSlotCount(); slotNo++) {
            if(m_sourceDeviceSlots[slotNo] == nullptr) {
                freeSlotNo = slotNo;
                break;
            }
        }
    }
    return freeSlotNo;
}

void cSourceDeviceManager::addSource(int slotPos, cSourceDevice *device)
{
    m_sourceDeviceSlots[slotPos] = device;
    m_activeSlotCount++;
    if(device->isDemo()) {
        m_demoCount++;
    }
    connect(device, &cSourceDevice::sigClosed, this, &cSourceDeviceManager::onSourceClosed);
}

bool cSourceDeviceManager::tryStartDemoDeviceRemove(int slotNo)
{
    bool removeStarted = false;
    cSourceDevice* source = m_sourceDeviceSlots[slotNo];
    if(source && source->isDemo()) {
        // Toggle close strategies for test: Call source close / Simulate USB serial removed
        m_removeDemoByDisconnect = !m_removeDemoByDisconnect;
        if(m_removeDemoByDisconnect) {
            static_cast<cSourceInterfaceDemo*>(source->getIoInterface().get())->simulateExternalDisconnect();
        }
        else {
            source->close();
        }
        removeStarted = true;
    }
    return removeStarted;
}


}
