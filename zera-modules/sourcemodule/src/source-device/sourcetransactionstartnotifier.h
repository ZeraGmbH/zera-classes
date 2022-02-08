#ifndef SOURCETRANSACTIONSTARTNOTIFIER_H
#define SOURCETRANSACTIONSTARTNOTIFIER_H

#include "sourceio.h"
#include <QObject>
#include <QSharedPointer>

class SourceTransactionStartNotifier : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<SourceTransactionStartNotifier> Ptr;
    SourceTransactionStartNotifier(ISourceIo::Ptr sourceIo);
    void startTransactionWithNotify(IoQueueEntry::Ptr transferGroup);
signals:
    void sigTransationStarted(int dataGroupId);

private:
    ISourceIo::Ptr m_sourceIo;
};

#endif // SOURCETRANSACTIONSTARTNOTIFIER_H
