#ifndef SOURCETRANSACTIONSTARTNOTIFIER_H
#define SOURCETRANSACTIONSTARTNOTIFIER_H

#include "sourcedevice.h"
#include <QObject>

class SourceTransactionStartNotifier : public QObject
{
    Q_OBJECT
public:
    SourceTransactionStartNotifier(ISourceDevice *sourceDevice);

    void startTransactionWithNotify(IoQueueEntry::Ptr transferGroup);
signals:
    void sigTransationStarted(int dataGroupId);

private:
    ISourceDevice *m_sourceDevice;
};

#endif // SOURCETRANSACTIONSTARTNOTIFIER_H
