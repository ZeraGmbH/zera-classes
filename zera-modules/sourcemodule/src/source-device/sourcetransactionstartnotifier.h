#ifndef SOURCETRANSACTIONSTARTNOTIFIER_H
#define SOURCETRANSACTIONSTARTNOTIFIER_H

#include "sourceio.h"
#include <QObject>

class SourceTransactionStartNotifier : public QObject
{
    Q_OBJECT
public:
    SourceTransactionStartNotifier(ISourceIo *sourceIo);

    void startTransactionWithNotify(IoQueueEntry::Ptr transferGroup);
signals:
    void sigTransationStarted(int dataGroupId);

private:
    ISourceIo *m_sourceIo;
};

#endif // SOURCETRANSACTIONSTARTNOTIFIER_H
