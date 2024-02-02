#ifndef SOURCESCANNERWITHINSTANCECOUNT_H
#define SOURCESCANNERWITHINSTANCECOUNT_H

#include "sourcescanner.h"

class SourceScannerWithInstanceCount : public SourceScanner
{
public:
    static Ptr create(IoDeviceBase::Ptr ioDevice, SourceScannerTemplate::Ptr ioStrategy, QUuid uuid=QUuid());
    static int getInstanceCount();
    virtual ~SourceScannerWithInstanceCount();
protected:
    SourceScannerWithInstanceCount(IoDeviceBase::Ptr ioDevice, SourceScannerTemplate::Ptr ioStrategy, QUuid uuid);
private:
    static int m_instanceCount;
};

#endif // SOURCESCANNERWITHINSTANCECOUNT_H
