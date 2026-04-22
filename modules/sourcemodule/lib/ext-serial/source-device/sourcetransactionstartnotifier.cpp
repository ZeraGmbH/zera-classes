#include "sourcetransactionstartnotifier.h"

SourceTransactionStartNotifier::SourceTransactionStartNotifier(AbstractSourceIoPtr sourceIo) :
    m_sourceIo(sourceIo)
{
}

AbstractSourceIoPtr SourceTransactionStartNotifier::getSourceIo()
{
    return m_sourceIo;
}

void SourceTransactionStartNotifier::startTransactionWithNotify(IoQueueGroup::Ptr transferGroup)
{
    m_sourceIo->startTransaction(transferGroup);
    emit sigTransationStarted(transferGroup->getGroupId());
}
