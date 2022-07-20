#include "clientactivecomponent.h"
#include <veinmoduleparameter.h>

ClientActiveComponent::ClientActiveComponent(QObject *parent) : QObject(parent)
{
    m_inactiveTimer.setSingleShot(true);
    connect(&m_inactiveTimer, &QTimer::timeout, this, &ClientActiveComponent::onClientTimeout);
}

void ClientActiveComponent::init(cVeinModuleParameter *component, int iInactiveTimeoutMs)
{
    if(m_pClientActiveNotifyPar) {
        disconnect(m_pClientActiveNotifyPar, &cVeinModuleParameter::sigValueChanged, this, &ClientActiveComponent::onClientNotification);
    }
    m_pClientActiveNotifyPar = component;
    connect(m_pClientActiveNotifyPar, &cVeinModuleParameter::sigValueChanged, this, &ClientActiveComponent::onClientNotification);
    m_inactiveTimer.setInterval(iInactiveTimeoutMs);
    m_bClientActive = false;
}

void ClientActiveComponent::onClientNotification(QVariant)
{
    m_inactiveTimer.stop(); // singleshot: start from beginning
    m_inactiveTimer.start();
    if(!m_bClientActive) {
        m_bClientActive = true;
        emit clientActiveStateChanged(m_bClientActive);
    }
}

void ClientActiveComponent::onClientTimeout()
{
    if(m_bClientActive) {
        m_bClientActive = false;
        emit clientActiveStateChanged(m_bClientActive);
    }
}
