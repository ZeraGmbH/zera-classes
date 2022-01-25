#include "sourcetransactionstartnotifier.h"

SourceTransactionStartNotifier::SourceTransactionStartNotifier(SourceDeviceInterface *sourceDevice) :
    m_sourceDevice(sourceDevice)
{
}

void SourceTransactionStartNotifier::startTransactionWithNotify(IoTransferDataGroup::Ptr transferGroup)
{
    m_sourceDevice->startTransaction(transferGroup);
    emit sigTransationStarted(transferGroup->getGroupId());
}
