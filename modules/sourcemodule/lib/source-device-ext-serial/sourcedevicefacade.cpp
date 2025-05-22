#include "sourcedevicefacade.h"
#include <vfmoduleparameter.h>
#include <jsonparamvalidator.h>
#include "sourceswitchjson.h"
#include <QVariant>

SourceDeviceFacade::SourceDeviceFacade(IoDeviceBase::Ptr ioDevice, SourceProperties properties) :
    SourceDeviceFacadeTemplate(ioDevice, ISourceIo::Ptr(new SourceIo(ioDevice, properties))),
    m_transactionNotifierStatus(SourceTransactionStartNotifier::Ptr::create(m_sourceIo)),
    m_transactionNotifierSwitch(SourceTransactionStartNotifier::Ptr::create(m_sourceIo)),
    m_statePoller(SourceStatePeriodicPoller::Ptr::create(m_transactionNotifierStatus)),
    m_stateController(m_transactionNotifierSwitch, m_transactionNotifierStatus, m_statePoller)
{
    m_switcher = std::make_unique<SourceSwitchJson>(m_sourceIo, m_transactionNotifierSwitch);
    connect(&m_stateController, &SourceStateController::sigStateChanged,
            this, &SourceDeviceFacade::onSourceStateChanged);
    connect(m_switcher.get(), &SourceSwitchJson::sigSwitchFinished,
            this, &SourceDeviceFacade::onSourceSwitchFinished);
    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, this,
            &SourceDeviceFacade::onIoDeviceClosed);
}

bool SourceDeviceFacade::close(QUuid uuid)
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

void SourceDeviceFacade::onSourceSwitchFinished()
{
    setVeinParamState(m_switcher->getCurrLoadState().getParams());
    if(m_closeRequested) {
        doFinalCloseActivities();
    }
}

void SourceDeviceFacade::onIoDeviceClosed()
{
    doFinalCloseActivities();
}

void SourceDeviceFacade::doFinalCloseActivities()
{
    resetVeinComponents();
    emit sigClosed(getId(), m_closeUuid);
}

void SourceDeviceFacade::setStatusPollTime(int ms)
{
    m_statePoller->setPollTime(ms);
}

void SourceDeviceFacade::enableCloseRequested(QUuid uuid)
{
    m_closeRequested = true;
    m_closeUuid = uuid;
}

bool SourceDeviceFacade::m_demoCloseByUsbDisconnect = false;

bool SourceDeviceFacade::tryDemoCloseByUsbDisconnect(QUuid uuid)
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
