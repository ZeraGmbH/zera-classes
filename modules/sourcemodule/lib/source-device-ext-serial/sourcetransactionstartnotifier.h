#ifndef SOURCETRANSACTIONSTARTNOTIFIER_H
#define SOURCETRANSACTIONSTARTNOTIFIER_H

#include "abstractsourceio.h"
#include <QObject>

class SourceTransactionStartNotifier : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<SourceTransactionStartNotifier> Ptr;
    SourceTransactionStartNotifier(AbstractSourceIoPtr sourceIo);
    AbstractSourceIoPtr getSourceIo();
    void startTransactionWithNotify(IoQueueGroup::Ptr transferGroup);
signals:
    void sigTransationStarted(int dataGroupId);

private:
    AbstractSourceIoPtr m_sourceIo;
};

#endif // SOURCETRANSACTIONSTARTNOTIFIER_H
