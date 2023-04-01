#ifndef SOURCESCANNERSTRATEGY_H
#define SOURCESCANNERSTRATEGY_H

#include "ioqueuegroup.h"
#include "ioqueuegrouplistfind.h"
#include "sourceproperties.h"
#include <memory>

class SourceScannerTemplate
{
public:
    typedef std::unique_ptr<SourceScannerTemplate> Ptr;
    IoQueueGroup::Ptr getNextQueueGroupForScan();
    SourceProperties findSourceFromResponse();
protected:
    virtual SourceProperties evalResponses(IoQueueGroup::Ptr ioGroup) = 0;
    IoQueueGroupListPtr m_scanIoGroupList;
private:
    IoQueueGroup::Ptr m_currIoGroup;
};

#endif // SOURCESCANNERSTRATEGY_H
