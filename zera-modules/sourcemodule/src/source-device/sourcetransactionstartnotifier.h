#ifndef SOURCETRANSACTIONSTARTNOTIFIER_H
#define SOURCETRANSACTIONSTARTNOTIFIER_H

#include "sourcedevice.h"
#include <QObject>

class SourceTransactionStartNotifier : public QObject
{
    Q_OBJECT
public:
    SourceTransactionStartNotifier(SourceDeviceInterface *sourceDevice);

    void startTransactionWithNotify(IoTransferDataGroup::Ptr transferGroup);
signals:
    void sigTransationStarted(int dataGroupId);

private:
    SourceDeviceInterface *m_sourceDevice;
};

#endif // SOURCETRANSACTIONSTARTNOTIFIER_H
