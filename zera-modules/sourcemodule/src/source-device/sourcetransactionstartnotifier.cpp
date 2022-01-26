#include "sourcetransactionstartnotifier.h"

SourceTransactionStartNotifier::SourceTransactionStartNotifier(ISourceDevice *sourceDevice) :
    m_sourceDevice(sourceDevice)
{
}

void SourceTransactionStartNotifier::startTransactionWithNotify(IoQueueEntry::Ptr transferGroup)
{
    m_sourceDevice->startTransaction(transferGroup);
    emit sigTransationStarted(transferGroup->getGroupId());
}
