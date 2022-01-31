#include "sourcedevicemanager.h"
#include "sourcedevicevein.h"
#include "io-device/iodevicefactory.h"
#include "io-device/iodevicedemo.h"
#include "source-scanner/sourcescanneriodemo.h"
#include "source-scanner/sourcescanneriozeraserial.h"
#include <random>

// DEMO helper
static bool randomBool() {
    // https://stackoverflow.com/questions/43329352/generating-random-boolean/43329456
    static auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    return gen();
}

SourceDeviceManager::SourceDeviceManager(int countSlots, QObject *parent) :
    QObject(parent),
    m_sourceDeviceSlots(QVector<SourceDeviceVein*>(countSlots, nullptr))
{
    connect(this, &SourceDeviceManager::sigSlotRemovedQueued,
            this, &SourceDeviceManager::sigSlotRemoved, Qt::QueuedConnection);
}

void SourceDeviceManager::startSourceScan(const IoDeviceTypes ioDeviceType, const QString deviceInfo, const QUuid uuid)
{
    if(m_activeSlotCount >= m_sourceDeviceSlots.count()) {
        emit sigSourceScanFinished(-1, uuid, QStringLiteral("No free slots"));
        return;
    }
    // factory for list of scanners? / get rid of ioDeviceType?
    bool started = false;
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(ioDeviceType);
    if(ioDevice) {
        started = ioDevice->open(deviceInfo);
        if(started) {
            ISourceScannerStrategy::Ptr scannerStrategy;
            if(ioDeviceType == IoDeviceTypes::DEMO) {
                static_cast<IoDeviceDemo*>(ioDevice.get())->setResponseDelay(true, 0);
                scannerStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
            }
            else {
                scannerStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoZeraSerial);
            }
            SourceScanner::Ptr sourceScanner = SourceScanner::create(ioDevice, scannerStrategy, uuid);
            connect(sourceScanner.get(), &SourceScanner::sigScanFinished,
                    this, &SourceDeviceManager::onScanFinished,
                    Qt::QueuedConnection);
            sourceScanner->startScan();
        }
    }
    if(!started) {
        emit sigSourceScanFinished(-1, uuid, QStringLiteral("Could not open IO-device"));
    }
}

void SourceDeviceManager::setDemoCount(int count)
{
    int demoDiff = count - getDemoCount();
    if(demoDiff > 0) { // add
        while(demoDiff && m_activeSlotCount < getSlotCount()) {
            startSourceScan(IoDeviceTypes::DEMO, "Demo", QUuid::createUuid());
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

void SourceDeviceManager::closeSource(int slotNo, const QUuid uuid)
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

void SourceDeviceManager::closeSource(QString ioDeviceInfo, const QUuid uuid)
{
    for(int slot = 0; slot<getSlotCount(); ++slot) {
        SourceDeviceVein* sourceDevice = getSourceDevice(slot);
        if(sourceDevice && sourceDevice->getIoDeviceInfo() == ioDeviceInfo) {
            closeSource(slot, uuid);
            return;
        }
    }
    emit sigSlotRemovedQueued(-1, uuid, QString("No source found at %1").arg(ioDeviceInfo));
}

int SourceDeviceManager::getSlotCount()
{
    return m_sourceDeviceSlots.size();
}

int SourceDeviceManager::getActiveSlotCount()
{
    return m_activeSlotCount;
}

int SourceDeviceManager::getDemoCount()
{
    int demoCount = 0;
    for(auto slot: m_sourceDeviceSlots) {
        if(slot && slot->isDemo()) {
            demoCount++;
        }
    }
    return demoCount;
}

SourceDeviceVein *SourceDeviceManager::getSourceDevice(int slotNo)
{
    SourceDeviceVein* getSourceDevice = nullptr;
    if(isValidSlotNo(slotNo)) {
        getSourceDevice = m_sourceDeviceSlots.at(slotNo);
    }
    return getSourceDevice;
}

void SourceDeviceManager::onScanFinished(SourceScanner::Ptr scanner)
{
    disconnect(scanner.get(), &SourceScanner::sigScanFinished, this, &SourceDeviceManager::onScanFinished);
    SourceProperties props = scanner->getSourcePropertiesFound();
    QString erorDesc;
    int freeSlot = findFreeSlot();
    SourceDeviceVein* sourceDeviceFound = nullptr;
    if(props.isValid()) {
        if(freeSlot >= 0) {
            sourceDeviceFound = new SourceDeviceVein(scanner->getIoDevice(), scanner->getSourcePropertiesFound());
            addSource(freeSlot, sourceDeviceFound);
        }
        else {
            erorDesc = QStringLiteral("Slots full");
        }
    }
    else {
        erorDesc = QStringLiteral("No source device found");
    }
    emit sigSourceScanFinished(freeSlot, scanner->getUuid(), erorDesc);
}

void SourceDeviceManager::onSourceClosed(SourceDeviceVein *sourceDevice, QUuid uuid)
{
    for(int slotNo=0; slotNo<m_sourceDeviceSlots.count(); slotNo++) {
        auto &sourceDeviceCurr = m_sourceDeviceSlots[slotNo];
        if(sourceDeviceCurr && sourceDeviceCurr == sourceDevice) {
            QString lastError = sourceDeviceCurr->getLastErrors().join(" / ");
            m_activeSlotCount--;
            disconnect(sourceDeviceCurr, &SourceDeviceVein::sigClosed, this, &SourceDeviceManager::onSourceClosed);
            delete sourceDeviceCurr;
            sourceDeviceCurr = nullptr;
            emit sigSlotRemovedQueued(slotNo, uuid, lastError);
            break;
        }
    }
}

bool SourceDeviceManager::isValidSlotNo(int slotNo)
{
    return slotNo >= 0 && slotNo<m_sourceDeviceSlots.count();
}

int SourceDeviceManager::findFreeSlot()
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

void SourceDeviceManager::addSource(int slotPos, SourceDeviceVein *device)
{
    m_sourceDeviceSlots[slotPos] = device;
    m_activeSlotCount++;
    connect(device, &SourceDeviceVein::sigClosed, this, &SourceDeviceManager::onSourceClosed);
}

bool SourceDeviceManager::tryStartDemoDeviceRemove(int slotNo)
{
    bool removeStarted = false;
    SourceDeviceVein* source = m_sourceDeviceSlots[slotNo];
    if(source && source->isDemo()) {
        source->close(QUuid());
        removeStarted = true;
    }
    return removeStarted;
}

