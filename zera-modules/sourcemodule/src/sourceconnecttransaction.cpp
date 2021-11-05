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
    connect(&m_demoTimer, &QTimer::timeout, this, &cSourceConnectTransaction::demoConnectTimeout);
}


cSourceDevice *cSourceConnectTransaction::sourceDeviceFound()
{
    return m_sourceDeviceIdentified;
}


void cSourceConnectTransaction::startConnect()
{
    switch(m_IOInterface->type()) {
    case SOURCE_INTERFACE_DEMO:
        m_demoTimer.start(1000);
        break;

    case SOURCE_INTERFACE_ASYNCSERIAL:
        // TODO
        Q_ASSERT(false);
        break;
    }
}


void cSourceConnectTransaction::demoConnectTimeout()
{
    m_sourceDeviceIdentified = new cSourceDevice(m_IOInterface, cSourceDevice::SOURCE_DEMO);
    emit sigTransactionFinished(this);
}


} // namespace
