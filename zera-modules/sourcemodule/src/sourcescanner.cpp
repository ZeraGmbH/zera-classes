#include "sourcescanner.h"
#include "sourceinterface.h"
#include <QUuid>

namespace SOURCEMODULE
{

tSourceScannerShPtr cSourceScanner::createScanner(tSourceInterfaceShPtr interface, QUuid uuid)
{
    tSourceScannerShPtr scanner = tSourceScannerShPtr(new cSourceScanner(interface, uuid));
    scanner->m_safePoinerOnThis = scanner;
    return scanner;
}

int cSourceScanner::m_InstanceCount = 0;

cSourceScanner::cSourceScanner(tSourceInterfaceShPtr interface, QUuid uuid) :
    QObject(nullptr),
    m_ioInterface(interface),
    m_uuid(uuid),
    m_sourceDeviceIdentified(nullptr)
{
    m_InstanceCount++;
    connect(m_ioInterface.get(), &cSourceInterfaceBase::sigIoFinished,
            this, &cSourceScanner::onIoFinished);
}

cSourceScanner::~cSourceScanner()
{
    m_InstanceCount--;
}

struct deviceResponseTypePair
{
    deviceResponseTypePair(QByteArray expectedResponse, SupportedSourceTypes sourceType) {
        this->expectedResponse = expectedResponse;
        this->sourceType = sourceType;
    }
    QByteArray expectedResponse;
    SupportedSourceTypes sourceType;
};

struct deviceDetectInfo
{
    deviceDetectInfo(QByteArray queryStr, QList<deviceResponseTypePair> responseTypePairs) {
        this->queryStr = queryStr;
        this->responseTypePairs = responseTypePairs;
    }
    QByteArray queryStr;
    QList<deviceResponseTypePair> responseTypePairs;
};

// Later (never hopefully) we might need multiple scan lists e.g bound to interface type or
// settable...
static QList<deviceDetectInfo> deviceScanListSerial = QList<deviceDetectInfo>()
    << deviceDetectInfo("STS\r", QList<deviceResponseTypePair> ()
                        // Sequence: special -> common
                        << deviceResponseTypePair("STSMT400", SOURCE_MT_CURRENT_ONLY)
                        << deviceResponseTypePair("STSMT", SOURCE_MT_COMMON)
                        << deviceResponseTypePair("STSFGMT", SOURCE_MT_COMMON))
    // TODO: guessworked initial data
    << deviceDetectInfo("TS\r", QList<deviceResponseTypePair> ()
                        << deviceResponseTypePair("TSFG", SOURCE_DEMO_FG_4PHASE))
       ;

cSourceDevice *cSourceScanner::getSourceDeviceFound()
{
    return m_sourceDeviceIdentified;
}

QUuid cSourceScanner::getUuid()
{
    return m_uuid;
}

int cSourceScanner::getInstanceCount()
{
    return m_InstanceCount;
}

void cSourceScanner::sendReceiveSourceID()
{
    deviceDetectInfo deviceDetectInfoCurrent = deviceScanListSerial[m_currentSourceTested];
    m_bytesReceived.clear();
    QByteArray bytesSend = createInterfaceSpecificPrepend() + deviceDetectInfoCurrent.queryStr;
    m_ioId = m_ioInterface->sendAndReceive(bytesSend, &m_bytesReceived);
}

QByteArray cSourceScanner::createInterfaceSpecificPrepend()
{
    QByteArray prepend;
    if(m_ioInterface->type() == SOURCE_INTERFACE_ASYNCSERIAL) {
        static_cast<cSourceInterfaceZeraSerial*>(m_ioInterface.get())->setBlockEndCriteriaNextIo();
        // clean hung up blockers on first try by prepending '\r'
        if(m_currentSourceTested == 0) {
            prepend = "\r";
        }
    }
    return prepend;
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

SupportedSourceTypes cSourceScanner::nextDemoType() {
    static SupportedSourceTypes sDemoTypeCounter(SOURCE_TYPE_COUNT);
    int nextDemoType = sDemoTypeCounter;
    nextDemoType++;
    if(nextDemoType >= SOURCE_TYPE_COUNT) {
        nextDemoType = 0;
    }
    sDemoTypeCounter = SupportedSourceTypes(nextDemoType);
    return sDemoTypeCounter;
}

void cSourceScanner::onIoFinished(int ioId, bool error)
{
    if(ioId != m_ioId) {
        qCritical("Are there multiple clients on scanner interface?");
        return;
    }
    bool validFound = false;
    bool moreChances = m_currentSourceTested < deviceScanListSerial.size()-1;
    bool ourJobIsDone = false;
    deviceDetectInfo deviceDetectInfoCurrent = deviceScanListSerial[m_currentSourceTested];
    SupportedSourceTypes sourceTypeFound = SOURCE_MT_COMMON;
    if(!error) {
        if(!m_ioInterface->isDemo()) {
            for(auto responseTypePair : deviceDetectInfoCurrent.responseTypePairs) {
                if(m_bytesReceived.contains(responseTypePair.expectedResponse)) {
                    validFound = true;
                    sourceTypeFound = responseTypePair.sourceType;
                    break;
                }
            }
        }
        else {
            validFound = !moreChances;
        }
    }
    if(validFound) {
        QByteArray version;
        if(m_ioInterface->isDemo()) {
            sourceTypeFound = nextDemoType();
            version = "V42";
        }
        else {
            version = extractVersionFromResponse(sourceTypeFound);
        }
        m_sourceDeviceIdentified = new cSourceDevice(m_ioInterface, sourceTypeFound, version);

        emit sigScanFinished(m_safePoinerOnThis);
        ourJobIsDone = true;
    }
    else if(!error && moreChances) {
        m_currentSourceTested++;
        sendReceiveSourceID(); // delay??
    }
    else {
        emit sigScanFinished(m_safePoinerOnThis); // notify: we failed :(
        ourJobIsDone = true;
    }
    if(ourJobIsDone) {
        // we are ready to die
        m_safePoinerOnThis = nullptr;
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
