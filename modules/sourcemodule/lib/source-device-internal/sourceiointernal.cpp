#include "sourceiointernal.h"

SourceIoInternal::SourceIoInternal(const QJsonObject capabilities) :
    m_capabilities(capabilities)
{
}

int SourceIoInternal::startTransaction(IoQueueGroup::Ptr transferGroup)
{

}

QJsonObject SourceIoInternal::getCapabilities() const
{
    return m_capabilities;
}
