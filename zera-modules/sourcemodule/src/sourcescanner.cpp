#include "sourcescanner.h"
#include "io-device/iodevicebrokendummy.h"
#include "io-device/iodevicezeraserial.h"
#include "io-device/iotransferdatasinglefactory.h"
#include <QUuid>

tSourceScannerShPtr SourceScanner::createScanner(tIoDeviceShPtr interface, QUuid uuid)
{
    tSourceScannerShPtr scanner = tSourceScannerShPtr(new SourceScanner(interface, uuid));
    scanner->m_safePoinerOnThis = scanner;
    return scanner;
}

int SourceScanner::m_InstanceCount = 0;

SourceScanner::SourceScanner(tIoDeviceShPtr interface, QUuid uuid) :
    QObject(nullptr),
    m_ioInterface(interface),
    m_uuid(uuid),
    m_sourceDeviceIdentified(nullptr),
    m_ioDataSingle(nullptr)
{
    m_InstanceCount++;
    connect(m_ioInterface.get(), &IoDeviceBrokenDummy::sigIoFinished,
            this, &SourceScanner::onIoFinished);
}

SourceScanner::~SourceScanner()
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

SourceDeviceVein *SourceScanner::getSourceDeviceFound()
{
    return m_sourceDeviceIdentified;
}

QUuid SourceScanner::getUuid()
{
    return m_uuid;
}

int SourceScanner::getInstanceCount()
{
    return m_InstanceCount;
}

void SourceScanner::sendReceiveSourceID()
{
    deviceDetectInfo deviceDetectInfoCurrent = deviceScanListSerial[m_currentSourceTested];
    QByteArray bytesSend = createInterfaceSpecificPrepend() + deviceDetectInfoCurrent.queryStr;
    m_ioDataSingle = IoTransferDataSingleFactory::createIoData(bytesSend, "");
    int ioId = m_ioInterface->sendAndReceive(m_ioDataSingle);
    m_currIoId.setCurrent(ioId);
}

QByteArray SourceScanner::createInterfaceSpecificPrepend()
{
    QByteArray prepend;
    if(m_ioInterface->type() == SERIAL_DEVICE_ASYNCSERIAL) {
        static_cast<IoDeviceZeraSerial*>(m_ioInterface.get())->setBlockEndCriteriaNextIo();
        // clean hung up blockers on first try by prepending '\r'
        if(m_currentSourceTested == 0) {
            prepend = "\r";
        }
    }
    return prepend;
}

QByteArray SourceScanner::extractVersionFromResponse(SupportedSourceTypes /* not used yet */)
{
    int pos;
    QByteArray bytesReceived = m_ioDataSingle->getDataReceived();
    for(pos=bytesReceived.length()-1; pos>=0; --pos) {
        QByteRef curr = bytesReceived[pos];
        if(curr == 'v' || curr == 'V') {
            break;
        }
    }
    QByteArray version;
    if(pos >= 0) {
        QByteArray termList(" \t\n\r");
        for(;pos < bytesReceived.length() && !termList.contains(bytesReceived[pos]); pos++) {
            version.append(bytesReceived[pos]);
        }
    }
    return version;
}

QByteArray SourceScanner::extractNameFromResponse(QByteArray responsePrefix, QByteArray version, SupportedSourceTypes)
{
    QByteArray name = m_ioDataSingle->getDataReceived();
    if(name.startsWith("STSFGMT")) {
        name = "MT3000";
    }
    else {
        name = name.replace(responsePrefix, "");
        name = name.replace(version, "");
        name = name.replace(" " , "");
        name = name.replace("\r" , "");
    }
    return name;
}

SupportedSourceTypes SourceScanner::nextDemoType() {
    static SupportedSourceTypes sDemoTypeCounter(SOURCE_TYPE_COUNT);
    int nextDemoType = sDemoTypeCounter;
    nextDemoType++;
    if(nextDemoType >= SOURCE_TYPE_COUNT) {
        nextDemoType = 0;
    }
    sDemoTypeCounter = SupportedSourceTypes(nextDemoType);
    return sDemoTypeCounter;
}

void SourceScanner::onIoFinished(int ioId, bool ioDeviceError)
{
    if(!m_currIoId.isCurrAndDeactivateIf(ioId)) {
        qCritical("Are there multiple clients on scanner interface?");
        return;
    }
    bool validFound = false;
    bool moreChances = m_currentSourceTested < deviceScanListSerial.size()-1;
    bool ourJobIsDone = false;
    deviceDetectInfo deviceDetectInfoCurrent = deviceScanListSerial[m_currentSourceTested];
    SupportedSourceTypes sourceTypeFound = SOURCE_MT_COMMON;
    QByteArray responsePrefix;
    if(!ioDeviceError) {
        if(!m_ioInterface->isDemo()) {
            for(auto responseTypePair : deviceDetectInfoCurrent.responseTypePairs) {
                if(m_ioDataSingle->getDataReceived().contains(responseTypePair.expectedResponse)) {
                    validFound = true;
                    sourceTypeFound = responseTypePair.sourceType;
                    responsePrefix = deviceDetectInfoCurrent.queryStr.replace("\r", "");
                    break;
                }
            }
        }
        else {
            validFound = !moreChances;
        }
    }
    if(validFound) {
        QByteArray deviceVersion;
        QByteArray deviceName;
        if(m_ioInterface->isDemo()) {
            sourceTypeFound = nextDemoType();
        }
        else {
            deviceVersion = extractVersionFromResponse(sourceTypeFound);
            deviceName = extractNameFromResponse(responsePrefix, deviceVersion, sourceTypeFound);
        }
        m_sourceDeviceIdentified = new SourceDeviceVein(m_ioInterface, sourceTypeFound, deviceName, deviceVersion);

        emit sigScanFinished(m_safePoinerOnThis);
        ourJobIsDone = true;
    }
    else if(!ioDeviceError && moreChances) {
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

void SourceScanner::startScan()
{
    m_currentSourceTested = 0;
    if(m_sourceDeviceIdentified) {
        // we are one shot
        qCritical("Do not call SourceScanner::startScan more than once per instance!");
        delete m_sourceDeviceIdentified;
        m_sourceDeviceIdentified = nullptr;
    }
    sendReceiveSourceID();
}
