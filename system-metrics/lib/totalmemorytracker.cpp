#include "totalmemorytracker.h"
#include "procmeminfodecoder.h"

void TotalMemoryTracker::CalculateMemoryUsedPercent()
{
    MemoryValues memoryValues = ProcMeminfoDecoder::getCurrentMemoryValues();
    quint32 usedMemory = memoryValues.totalMemory - memoryValues.freeMemory;
    quint32 buffersAndCachedMemory = memoryValues.buffers + memoryValues.cached;
    quint32 usedRAM = usedMemory - buffersAndCachedMemory;

    m_memoryUsageParams.m_RAMUsedPercent = memoryValues.totalMemory == 0.0 ? 0.0 : float(usedRAM) / float(memoryValues.totalMemory) * 100.0;
    m_memoryUsageParams.m_buffersAndCachedUsedPercent = memoryValues.totalMemory == 0.0 ? 0.0 : float(buffersAndCachedMemory) / float(memoryValues.totalMemory) * 100.0;
}

memoryUsageParams TotalMemoryTracker::getMemoryUsageParams() const
{
    return m_memoryUsageParams;
}

void TotalMemoryTracker::periodicLogs()
{
    qInfo("Memory used: %0.2f%%, Buffers & Cache: %0.2f%% ", m_memoryUsageParams.m_RAMUsedPercent, m_memoryUsageParams.m_buffersAndCachedUsedPercent);
}
