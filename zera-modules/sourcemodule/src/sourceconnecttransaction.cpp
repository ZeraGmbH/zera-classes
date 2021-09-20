#include "sourceconnecttransaction.h"
#include "iointerface.h"
#include "sourcedevice.h"

namespace SOURCEMODULE
{

cSourceConnectTransaction::cSourceConnectTransaction(cIOInterface *interface, QObject *parent) :
    QObject(parent),
    m_IOInterface(interface),
    m_sourceDeviceIdentified(nullptr)
{
    connect(&m_demoTimer, &QTimer::timeout, this, &cSourceConnectTransaction::demoConnectTimeout);
}


cSourceDevice *cSourceConnectTransaction::sourceDeviceFound()
{
    return m_sourceDeviceIdentified;
}


void cSourceConnectTransaction::startConnect()
{
    switch(m_IOInterface->type()) {
    case cIOInterface::IO_DEMO:
        m_demoTimer.start(1000);
        break;

    case cIOInterface::IO_ASYNC_SERIAL:
        // TODO
        Q_ASSERT(false);
        break;
    }
}


void cSourceConnectTransaction::demoConnectTimeout()
{
    m_sourceDeviceIdentified = new cSourceDevice(m_IOInterface, cSourceDevice::SOURCE_DEMO);
    m_sourceDeviceIdentified->setName("Demo");
    emit sigTransactionFinished(this);
}


} // namespace
