#include "sourcescanner.h"
#include "sourceinterface.h"
#include <QUuid>

namespace SOURCEMODULE
{

cSourceScanner::cSourceScanner(tSourceInterfaceShPtr interface, QUuid uuid) :
    QObject(nullptr),
    m_ioInterface(interface),
    m_uuid(uuid),
    m_sourceDeviceIdentified(nullptr)
{
    connect(m_ioInterface.get(), &cSourceInterfaceBase::sigIoFinished, this, &cSourceScanner::onIoFinished);
}

cSourceScanner::~cSourceScanner()
{
}

struct deviceDetectInfo
{
    deviceDetectInfo(QByteArray queryStr, QByteArray expectedResponse, SupportedSourceTypes sourceType) {
        this->queryStr = queryStr;
        this->expectedResponse = expectedResponse;
        this->sourceType = sourceType;
    }
    QByteArray queryStr;
    QByteArray expectedResponse;
    SupportedSourceTypes sourceType;
};

// Later (never hopefully) we might need multiple scan lists e.g bound to interface type or
// settable...
static QList<deviceDetectInfo> deviceScanListSerial = QList<deviceDetectInfo>()
    // TODO: Regex?
    << deviceDetectInfo("STS\r", "STSFGMT", SOURCE_MT3000) // tested
    // TODO: guessworked initial data
    << deviceDetectInfo("TS\r", "MT400-20", SOURCE_MT400_20)
    << deviceDetectInfo("TS\r", "FG", SOURCE_MT3000)
    ;

cSourceDevice *cSourceScanner::getSourceDeviceFound()
{
    return m_sourceDeviceIdentified;
}

QUuid cSourceScanner::getUuid()
{
    return m_uuid;
}

void cSourceScanner::setScannerReference(tSourceScannerShPtr scannerReference)
{
    cSourceScanner* scannerToSet = scannerReference.get();
    if(scannerToSet && scannerToSet != this) {
        qFatal("Do not set reference to other scanner!");
    }
    m_scannerReference = scannerReference;
}

void cSourceScanner::sendReceiveSourceID()
{
    deviceDetectInfo deviceDetectInfoCurrent = deviceScanListSerial[m_currentSourceTested];
    m_bytesReceived.clear();
    // interface specifics
    QByteArray globalPrepend;
    if(m_ioInterface->type() == SOURCE_INTERFACE_ASYNCSERIAL) {
        static_cast<cSourceInterfaceZeraSerial*>(m_ioInterface.get())->setBlockEndCriteriaNextIo();
        // clean hung up blockers on first try by prepending '\r'
        if(m_currentSourceTested == 0) {
            globalPrepend = "\r";
        }
    }
    QByteArray bytesSend = globalPrepend + deviceDetectInfoCurrent.queryStr;
    m_ioInterface->sendAndReceive(bytesSend, &m_bytesReceived);
}

QByteArray cSourceScanner::extractVersionFromResponse(SupportedSourceTypes /* not used yet */)
{
    int pos;
    for(pos=m_bytesReceived.length()-1; pos>=0; --pos) {
        QByteRef curr = m_bytesReceived[pos];
        if(curr == 'v' || curr == 'V') {
            break;
        }
    }
    QByteArray version;
    if(pos >= 0) {
        QByteArray termList(" \t\n\r");
        for(;pos < m_bytesReceived.length() && !termList.contains(m_bytesReceived[pos]); pos++) {
            version.append(m_bytesReceived[pos]);
        }
    }
    return version;
}

static enum SupportedSourceTypes sDemoTypeCounter(SOURCE_TYPE_COUNT);

void cSourceScanner::onIoFinished(int ioID)
{
    bool validFound = false;
    bool moreChances = m_currentSourceTested < deviceScanListSerial.size()-1;
    bool ourJobIsDone = false;
    deviceDetectInfo deviceDetectInfoCurrent = deviceScanListSerial[0];
    if(ioID) { // receive id == 0 -> I/O problems -> don't try next
        if(m_ioInterface->type() != SOURCE_INTERFACE_DEMO) {
            deviceDetectInfoCurrent = deviceScanListSerial[m_currentSourceTested];
            if(m_bytesReceived.contains(deviceDetectInfoCurrent.expectedResponse)) {
                validFound = true;
            }
        }
        else {
            validFound = !moreChances;
        }
    }
    if(validFound) {
        QByteArray version;
        if(m_ioInterface->type() == SOURCE_INTERFACE_DEMO) {
            int nextDemoType = sDemoTypeCounter;
            nextDemoType++;
            if(nextDemoType >= SOURCE_TYPE_COUNT) {
                nextDemoType = 0;
            }
            deviceDetectInfoCurrent.sourceType = sDemoTypeCounter = SupportedSourceTypes(nextDemoType);
            version = "V42";
        }
        else {
            version = extractVersionFromResponse(deviceDetectInfoCurrent.sourceType);
        }
        m_sourceDeviceIdentified = new cSourceDevice(m_ioInterface, deviceDetectInfoCurrent.sourceType, version);

        emit sigScanFinished(m_scannerReference);
        ourJobIsDone = true;
    }
    else if(ioID && moreChances) {
        m_currentSourceTested++;
        sendReceiveSourceID(); // delay??
    }
    else {
        emit sigScanFinished(m_scannerReference); // notify: we failed :(
        ourJobIsDone = true;
    }
    if(ourJobIsDone) {
        // we are ready to die
        setScannerReference(nullptr);
    }
}

void cSourceScanner::startScan()
{
    m_currentSourceTested = 0;
    if(m_sourceDeviceIdentified) {
        // we are one shot
        qCritical("Do not call cSourceScanner::startScan more than once per instance!");
        delete m_sourceDeviceIdentified;
        m_sourceDeviceIdentified = nullptr;
    }
    sendReceiveSourceID();
}

} // namespace
