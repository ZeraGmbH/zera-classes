#include "sourceconnecttransaction.h"
#include "sourceinterface.h"
#include "sourcedevice.h"

namespace SOURCEMODULE
{

cSourceConnectTransaction::cSourceConnectTransaction(cSourceInterfaceBase *interface, QObject *parent) :
    QObject(parent),
    m_IOInterface(interface),
    m_sourceDeviceIdentified(nullptr)
{
    connect(m_IOInterface, &cSourceInterfaceBase::ioFinished, this, &cSourceConnectTransaction::onIoFinished);
}

struct deviceDetectInfo
{
    deviceDetectInfo(QByteArray prefix, QByteArray queryStr, QByteArray expectedResponse) {
        this->prefix = prefix;
        this->queryStr = queryStr;
        this->expectedResponse = expectedResponse;
    }
    QByteArray prefix;
    QByteArray queryStr;
    QByteArray expectedResponse;
};

// Later (never hopefully) we might need multiple scan lists e.g bound to interface type or
// settable...
static QList<deviceDetectInfo> deviceScanListSerial = QList<deviceDetectInfo>()
    // TODO: This is guessworked initial data so that we have something to test
    // TODO: Regex?
    << deviceDetectInfo("S", "TS", "MT")
    << deviceDetectInfo("", "TS", "MT")
    << deviceDetectInfo("", "TS", "FG")
    ;

cSourceDevice *cSourceConnectTransaction::sourceDeviceFound()
{
    return m_sourceDeviceIdentified;
}

void cSourceConnectTransaction::sendReceiveSourceID()
{
    deviceDetectInfo deviceDetectInfoCurrent = deviceScanListSerial[m_currentSourceTested];
    m_receivedData.clear();
    // interface specifics
    QByteArray globalPrepend;
    if(m_IOInterface->type() == SOURCE_INTERFACE_ASYNCSERIAL) {
        static_cast<cSourceInterfaceZeraSerial*>(m_IOInterface)->setBlockEndCriteriaNextIo();
        // clean hung up blockers on first try by prepending '\n'
        if(m_currentSourceTested == 0) {
            globalPrepend = "\n";
        }
    }
    QByteArray dataSend = globalPrepend + deviceDetectInfoCurrent.prefix + deviceDetectInfoCurrent.queryStr;
    m_IOInterface->sendAndReceive(dataSend, &m_receivedData);
}

void cSourceConnectTransaction::onIoFinished(int transactionID)
{
    bool validFound = false;
    bool moreChances = m_currentSourceTested < deviceScanListSerial.size()-1;
    if(transactionID) { // receiving transaction id = 0 -> I/O problems -> don't try next
        if(m_IOInterface->type() != SOURCE_INTERFACE_DEMO) {
            deviceDetectInfo deviceDetectInfoCurrent = deviceScanListSerial[m_currentSourceTested];
            if(m_receivedData.contains(deviceDetectInfoCurrent.expectedResponse)) {
                validFound = true;
            }
        }
        else { // demo try all
            validFound = !moreChances;
        }
    }
    if(validFound) {
        if(m_IOInterface->type() == SOURCE_INTERFACE_DEMO) {
            m_sourceDeviceIdentified = new cSourceDevice(m_IOInterface, cSourceDevice::SOURCE_DEMO);
        }
        else {
            // TODO
        }
        emit sigTransactionFinished(this);
    }
    else if(transactionID && moreChances) {
        m_currentSourceTested++;
        sendReceiveSourceID(); // delay??
    }
    else {
        emit sigTransactionFinished(this); // notify: we failed :(
    }
}

void cSourceConnectTransaction::startConnect()
{
    m_currentSourceTested = 0;
    if(m_sourceDeviceIdentified) {
        // we are one shot
        qCritical("Do not call cSourceConnectTransaction::startConnect more than once per instance!");
        delete m_sourceDeviceIdentified;
        m_sourceDeviceIdentified = nullptr;
    }
    sendReceiveSourceID();
}

} // namespace
