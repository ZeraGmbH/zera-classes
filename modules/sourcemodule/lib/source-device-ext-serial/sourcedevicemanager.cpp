#include "sourcedevicemanager.h"
#include "iodevicefactory.h"
#include "iodevicedemo.h"
#include "sourcescanneriodemo.h"
#include "sourcescanneriozeraserial.h"
#include "sourcedevicefacade.h"
#include <random>

// DEMO helper
static bool randomBool() {
    // https://stackoverflow.com/questions/43329352/generating-random-boolean/43329456
    static auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    return gen();
}

SourceDeviceManager::SourceDeviceManager(int countSlots, QObject *parent) :
    QObject(parent),
    m_sourceControllers(QVector<SourceDeviceFacadeTemplate::Ptr>(countSlots, nullptr)),
    m_activeSlots(QVector<bool>(countSlots, false))
{
    connect(this, &SourceDeviceManager::sigSlotRemovedQueued,
            this, &SourceDeviceManager::sigSlotRemoved, Qt::QueuedConnection);
}

void SourceDeviceManager::addInternalSource(QString sourceJsonStruct)
{

}

void SourceDeviceManager::startSourceScan(const IoDeviceTypes ioDeviceType, const QString deviceInfo, const QUuid uuid)
{
    if(getActiveSlotCount() >= m_sourceControllers.count()) {
        emit sigSourceScanFinished(-1, uuid, QStringLiteral("No free slots"));
        return;
    }
    // factory for list of scanners? / get rid of ioDeviceType?
    bool started = false;
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(ioDeviceType);
    if(ioDevice) {
        started = ioDevice->open(deviceInfo);
        if(started) {
            SourceScannerTemplate::Ptr scannerStrategy;
            if(ioDeviceType == IoDeviceTypes::DEMO) {
                IoDeviceDemo* demoIO = static_cast<IoDeviceDemo*>(ioDevice.get());
                demoIO->setResponseDelay(true, 0);
                if(deviceInfo.contains("broken", Qt::CaseInsensitive))
                    demoIO->setAllTransfersError(true);
                scannerStrategy = std::make_unique<SourceScannerIoDemo>();
            }
            else
                scannerStrategy = std::make_unique<SourceScannerIoZeraSerial>();
            SourceScanner::Ptr sourceScanner = SourceScanner::create(ioDevice, std::move(scannerStrategy), uuid);
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
        while(demoDiff && getActiveSlotCount() < getSlotCount()) {
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
    if(isValidSlotNo(slotNo) && m_activeSlots[slotNo])
        closeRequested = m_sourceControllers[slotNo]->close(uuid);
    if(!closeRequested)
        emit sigSlotRemovedQueued(-1, uuid, QStringLiteral("No source found at slot %1").arg(slotNo));
}

void SourceDeviceManager::closeSource(QString ioDeviceInfo, const QUuid uuid)
{
    for(int slot = 0; slot<getSlotCount(); ++slot) {
        SourceDeviceFacadeTemplate::Ptr sourceController = getSourceController(slot);
        if(sourceController && sourceController->getIoDeviceInfo() == ioDeviceInfo) {
            closeSource(slot, uuid);
            return;
        }
    }
    emit sigSlotRemovedQueued(-1, uuid, QString("No source found at %1").arg(ioDeviceInfo));
}

void SourceDeviceManager::closeAll()
{
    checkHandleAllClosed();
    for(int slot = 0; slot<getSlotCount(); ++slot) {
        SourceDeviceFacadeTemplate::Ptr sourceController = getSourceController(slot);
        if(sourceController) {
            closeSource(slot, QUuid());
        }
    }
}

int SourceDeviceManager::getSlotCount()
{
    return m_sourceControllers.size();
}

int SourceDeviceManager::getActiveSlotCount()
{
    int activeSlotCount = 0;
    for(auto slotActive : qAsConst(m_activeSlots))
        if(slotActive)
            activeSlotCount++;
    return activeSlotCount;
}

int SourceDeviceManager::getDemoCount()
{
    int demoCount = 0;
    for(int slotPos=0; slotPos<getSlotCount(); slotPos++) {
        if(m_activeSlots[slotPos]) {
            const auto &slot = m_sourceControllers[slotPos];
            if(slot->hasDemoIo())
                demoCount++;
        }
    }
    return demoCount;
}

SourceDeviceFacadeTemplate::Ptr SourceDeviceManager::getSourceController(int slotNo)
{
    SourceDeviceFacadeTemplate::Ptr sourceController;
    if(isValidSlotNo(slotNo) && m_activeSlots[slotNo])
        sourceController = m_sourceControllers.at(slotNo);
    return sourceController;
}

int SourceDeviceManager::tryAddSourceToFreeSlot(IoDeviceBase::Ptr ioDevice, SourceProperties props)
{
    int freeSlot = findFreeSlot();
    if(freeSlot >= 0)
        addSource(freeSlot, std::make_shared<SourceDeviceFacade>(ioDevice, props));
    return freeSlot;
}

void SourceDeviceManager::onScanFinished(SourceScanner::Ptr scanner)
{
    disconnect(scanner.get(), &SourceScanner::sigScanFinished, this, &SourceDeviceManager::onScanFinished);
    SourceProperties props = scanner->getSourcePropertiesFound();
    QString erorDesc;
    int freeSlot = -1;
    if(props.wasSet()) {
        freeSlot = tryAddSourceToFreeSlot(scanner->getIoDevice(), props);
        if(freeSlot < 0)
            erorDesc = QStringLiteral("Slots full");
    }
    else
        erorDesc = QStringLiteral("No source device found");
    emit sigSourceScanFinished(freeSlot, scanner->getUuid(), erorDesc);
}

void SourceDeviceManager::onSourceClosed(int facadeId, QUuid uuid)
{
    for(int slotNo=0; slotNo<m_sourceControllers.count(); slotNo++) {
        if(m_activeSlots[slotNo]) {
            auto &sourceController = m_sourceControllers[slotNo];
            if(sourceController->getId() == facadeId) {
                QString lastError = sourceController->getLastErrors().join(" / ");
                m_activeSlots[slotNo] = false;
                disconnect(sourceController.get(), &SourceDeviceFacadeTemplate::sigClosed,
                           this, &SourceDeviceManager::onSourceClosed);
                emit sigSlotRemovedQueued(slotNo, uuid, lastError);
                break;
            }
        }
    }
    checkHandleAllClosed();
}

bool SourceDeviceManager::isValidSlotNo(int slotNo)
{
    return slotNo >= 0 && slotNo < getSlotCount();
}

int SourceDeviceManager::findFreeSlot()
{
    int freeSlotNo = -1;
    if(getActiveSlotCount() < getSlotCount()) {
        for(int slotNo=0; slotNo<getSlotCount(); slotNo++) {
            if(!m_activeSlots[slotNo]) {
                freeSlotNo = slotNo;
                break;
            }
        }
    }
    return freeSlotNo;
}

void SourceDeviceManager::addSource(int slotPos, SourceDeviceFacadeTemplate::Ptr deviceController)
{
    m_sourceControllers[slotPos] = deviceController;
    m_activeSlots[slotPos] = true;
    connect(deviceController.get(), &SourceDeviceFacadeTemplate::sigClosed,
            this, &SourceDeviceManager::onSourceClosed);
}

bool SourceDeviceManager::tryStartDemoDeviceRemove(int slotNo)
{
    bool removeStarted = false;
    if(m_activeSlots[slotNo]) {
        SourceDeviceFacadeTemplate::Ptr source = m_sourceControllers[slotNo];
        if(source->hasDemoIo()) {
            source->close(QUuid());
            removeStarted = true;
        }
    }
    return removeStarted;
}

void SourceDeviceManager::checkHandleAllClosed()
{
    if(getActiveSlotCount() == 0) {
        emit sigAllSlotsRemoved();
    }
}

