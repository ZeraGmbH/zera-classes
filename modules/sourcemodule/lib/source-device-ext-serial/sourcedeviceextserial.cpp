#include "sourcedeviceextserial.h"
#include <vfmoduleparameter.h>
#include <jsonparamvalidator.h>
#include "sourceswitchjson.h"
#include <QVariant>

SourceDeviceExtSerial::SourceDeviceExtSerial(IoDeviceBase::Ptr ioDevice, SourceProperties properties) :
    SourceDeviceTemplate(ioDevice, std::make_shared<SourceIoExtSerial>(ioDevice, properties)),
    m_transactionNotifierStatus(SourceTransactionStartNotifier::Ptr::create(m_sourceIo)),
    m_transactionNotifierSwitch(SourceTransactionStartNotifier::Ptr::create(m_sourceIo)),
    m_statePoller(SourceStatePeriodicPoller::Ptr::create(m_transactionNotifierStatus)),
    m_stateController(m_transactionNotifierSwitch, m_transactionNotifierStatus, m_statePoller)
{
    m_switcher = std::make_unique<SourceSwitchJson>(m_sourceIo, m_transactionNotifierSwitch);
    connect(&m_stateController, &SourceStateController::sigStateChanged,
            this, &SourceDeviceExtSerial::onSourceStateChanged);
    connect(m_switcher.get(), &SourceSwitchJson::sigSwitchFinished,
            this, &SourceDeviceExtSerial::onSourceSwitchFinished);
    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, this,
            &SourceDeviceExtSerial::onIoDeviceClosed);
}

bool SourceDeviceExtSerial::close(QUuid uuid)
{
    bool closeRequested = tryDemoCloseByUsbDisconnect(uuid);
    if(!m_closeRequested && !closeRequested) {
        closeRequested = true;
        enableCloseRequested(uuid);
        m_statePoller->stopPeriodicPoll();
        if(m_switcher->getCurrLoadState().getOn())
            m_switcher->switchOff();
        else
            doFinalCloseActivities();
    }
    return closeRequested;
}

void SourceDeviceExtSerial::onSourceSwitchFinished()
{
    setVeinParamState(m_switcher->getCurrLoadState().getParams());
    if(m_closeRequested) {
        doFinalCloseActivities();
    }
}

void SourceDeviceExtSerial::onIoDeviceClosed()
{
    doFinalCloseActivities();
}

void SourceDeviceExtSerial::doFinalCloseActivities()
{
    resetVeinComponents();
    emit sigClosed(getId(), m_closeUuid);
}

void SourceDeviceExtSerial::setStatusPollTime(int ms)
{
    m_statePoller->setPollTime(ms);
}

void SourceDeviceExtSerial::enableCloseRequested(QUuid uuid)
{
    m_closeRequested = true;
    m_closeUuid = uuid;
}

bool SourceDeviceExtSerial::m_demoCloseByUsbDisconnect = false;

bool SourceDeviceExtSerial::tryDemoCloseByUsbDisconnect(QUuid uuid)
{
    bool closeRequested = false;
    if(hasDemoIo() && !m_closeRequested) {
        m_demoCloseByUsbDisconnect = !m_demoCloseByUsbDisconnect;
        if(m_demoCloseByUsbDisconnect) {
            m_closeRequested = true;
            closeRequested = true;
            enableCloseRequested(uuid);
            m_ioDevice->close();
        }
    }
    return closeRequested;
}
