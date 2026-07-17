#include "signalconnectiondelegateupdater.h"

namespace SCPIMODULE {

void SignalConnectionDelegateUpdater::addDelegate(cSignalConnectionDelegate *delegate)
{
    m_signalConnectionDelegates.append(delegate);
}

void SignalConnectionDelegateUpdater::removeDelegate(cSignalConnectionDelegate *delegate)
{
    m_signalConnectionDelegates.removeAll(delegate);
}

void SignalConnectionDelegateUpdater::updateDelegates(int entityId, const QString &componentName, const QVariant &newValue)
{
    for(int i = 0; i < m_signalConnectionDelegates.count(); i++) {
        cSignalConnectionDelegate* delegate = m_signalConnectionDelegates.at(i);
        if(delegate->EntityId() == entityId && delegate->ComponentName() == componentName)
            delegate->setStatus(newValue);
    }
}

}