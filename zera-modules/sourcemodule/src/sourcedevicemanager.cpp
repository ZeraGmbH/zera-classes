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
    m_sourceDeviceSlots(QVector<SourceDevice*>(countSlots, nullptr))
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
    int demoDiff = count - getDemoCount();
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

void cSourceDeviceManager::setDemoFollowsTimeout(bool follow)
{
    m_bDemoDelayFollowsTimeout = follow;
}

void cSourceDeviceManager::closeSource(int slotNo, const QUuid uuid)
{
    bool closeRequested = false;
    if(isValidSlotNo(slotNo)) {
        auto sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
        if(sourceDeviceCurr) {
            closeRequested = sourceDeviceCurr->close(uuid);
        }
    }
    if(!closeRequested) {
        emit sigSlotRemovedQueued(-1, uuid, QStringLiteral("No source found at slot %1").arg(slotNo));
    }
}

void cSourceDeviceManager::closeSource(QString interfaceDeviceInfo, const QUuid uuid)
{
    for(int slot = 0; slot<getSlotCount(); ++slot) {
        SourceDevice* sourceDevice = getSourceDevice(slot);
        if(sourceDevice && sourceDevice->getInterfaceDeviceInfo() == interfaceDeviceInfo) {
            closeSource(slot, uuid);
            return;
        }
    }
    emit sigSlotRemovedQueued(-1, uuid, QString("No source found at %1").arg(interfaceDeviceInfo));
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
    int demoCount = 0;
    for(auto slot: m_sourceDeviceSlots) {
        if(slot && slot->isDemo()) {
            demoCount++;
        }
    }
    return demoCount;
}

SourceDevice *cSourceDeviceManager::getSourceDevice(int slotNo)
{
    SourceDevice* getSourceDevice = nullptr;
    if(isValidSlotNo(slotNo)) {
        getSourceDevice = m_sourceDeviceSlots.at(slotNo);
    }
    return getSourceDevice;
}

void cSourceDeviceManager::onScanFinished(tSourceScannerShPtr scanner)
{
    disconnect(scanner.get(), &cSourceScanner::sigScanFinished, this, &cSourceDeviceManager::onScanFinished);

    SourceDevice *sourceDeviceFound = scanner->getSourceDeviceFound();
    QString erorDesc;
    int freeSlot = findFreeSlot();
    if(sourceDeviceFound) {
        if(freeSlot >= 0) {
            sourceDeviceFound->setDemoDelayFollowsTimeout(m_bDemoDelayFollowsTimeout);
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

void cSourceDeviceManager::onSourceClosed(SourceDevice *sourceDevice, QUuid uuid)
{
    for(int slotNo=0; slotNo<m_sourceDeviceSlots.count(); slotNo++) {
        auto &sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
        if(sourceDeviceCurr && sourceDeviceCurr == sourceDevice) {
            QString lastError = sourceDeviceCurr->getLastErrors().join(" / ");
            m_activeSlotCount--;
            disconnect(sourceDeviceCurr, &SourceDevice::sigClosed, this, &cSourceDeviceManager::onSourceClosed);
            delete sourceDeviceCurr;
            sourceDeviceCurr = nullptr;
            emit sigSlotRemovedQueued(slotNo, uuid, lastError);
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

void cSourceDeviceManager::addSource(int slotPos, SourceDevice *device)
{
    m_sourceDeviceSlots[slotPos] = device;
    m_activeSlotCount++;
    connect(device, &SourceDevice::sigClosed, this, &cSourceDeviceManager::onSourceClosed);
}

bool cSourceDeviceManager::tryStartDemoDeviceRemove(int slotNo)
{
    bool removeStarted = false;
    SourceDevice* source = m_sourceDeviceSlots[slotNo];
    if(source && source->isDemo()) {
        source->close(QUuid());
        removeStarted = true;
    }
    return removeStarted;
}


}
