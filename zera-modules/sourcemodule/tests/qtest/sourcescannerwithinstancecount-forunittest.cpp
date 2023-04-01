#include "sourcescannerwithinstancecount-forunittest.h"

int SourceScannerWithInstanceCount::m_instanceCount = 0;

SourceScannerWithInstanceCount::SourceScannerWithInstanceCount(IoDeviceBase::Ptr ioDevice, SourceScannerTemplate::Ptr ioStrategy, QUuid uuid) :
    SourceScanner(ioDevice, std::move(ioStrategy), uuid)
{
    m_instanceCount++;
}

SourceScanner::Ptr SourceScannerWithInstanceCount::create(IoDeviceBase::Ptr ioDevice, SourceScannerTemplate::Ptr ioStrategy, QUuid uuid)
{
    // This is more or less a copy SourceScanner::create and
    // creates SourceScannerWithInstanceCount instead of SourceScanner object.
    SourceScannerWithInstanceCount* rawInstancePointer = new SourceScannerWithInstanceCount(ioDevice, std::move(ioStrategy), uuid);
    SourceScanner::Ptr scanner = SourceScanner::Ptr(rawInstancePointer);
    rawInstancePointer->m_safePoinerOnThis = scanner;
    return scanner;
}

int SourceScannerWithInstanceCount::getInstanceCount()
{
    return m_instanceCount;
}

SourceScannerWithInstanceCount::~SourceScannerWithInstanceCount()
{
    m_instanceCount--;
}

