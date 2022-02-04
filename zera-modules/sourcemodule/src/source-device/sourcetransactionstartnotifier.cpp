#include "sourcetransactionstartnotifier.h"

SourceTransactionStartNotifier::SourceTransactionStartNotifier(ISourceIo *sourceIo) :
    m_sourceIo(sourceIo)
{
}

void SourceTransactionStartNotifier::startTransactionWithNotify(IoQueueEntry::Ptr transferGroup)
{
    m_sourceIo->startTransaction(transferGroup);
    emit sigTransationStarted(transferGroup->getGroupId());
}
