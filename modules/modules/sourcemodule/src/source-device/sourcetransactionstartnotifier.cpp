#include "sourcetransactionstartnotifier.h"

SourceTransactionStartNotifier::SourceTransactionStartNotifier(ISourceIo::Ptr sourceIo) :
    m_sourceIo(sourceIo)
{
}

ISourceIo::Ptr SourceTransactionStartNotifier::getSourceIo()
{
    return m_sourceIo;
}

void SourceTransactionStartNotifier::startTransactionWithNotify(IoQueueGroup::Ptr transferGroup)
{
    m_sourceIo->startTransaction(transferGroup);
    emit sigTransationStarted(transferGroup->getGroupId());
}
