#include "sourcescannertemplate.h"

IoQueueGroup::Ptr SourceScannerTemplate::getNextQueueGroupForScan()
{
    Q_ASSERT(!m_currIoGroup);
    if(!m_scanIoGroupList.isEmpty())
        m_currIoGroup = m_scanIoGroupList.takeFirst();
    return m_currIoGroup;
}

SourceProperties SourceScannerTemplate::findSourceFromResponse()
{
    Q_ASSERT(m_currIoGroup);
    SourceProperties sourceProps = evalResponses(m_currIoGroup);
    m_currIoGroup = nullptr;
    return sourceProps;
}
