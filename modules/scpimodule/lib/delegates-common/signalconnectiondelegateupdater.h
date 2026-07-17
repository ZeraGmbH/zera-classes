#ifndef SIGNALCONNECTIONDELEGATEUPDATER_H
#define SIGNALCONNECTIONDELEGATEUPDATER_H

#include "signalconnectiondelegate.h"
#include <QList>

namespace SCPIMODULE {

class SignalConnectionDelegateUpdater
{
public:
    void addDelegate(cSignalConnectionDelegate* delegate);
    void removeDelegate(cSignalConnectionDelegate *delegate);
    void updateDelegates(int entityId, const QString &componentName, const QVariant &newValue);

private:
    QList<cSignalConnectionDelegate*> m_signalConnectionDelegates;
};

}
#endif // SIGNALCONNECTIONDELEGATEUPDATER_H
