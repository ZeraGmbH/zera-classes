#include "totalmemorytracker.h"
#include "procmeminfodecoder.h"

void TotalMemoryTracker::CalculateMemoryUsedPercent()
{
    MemoryValues memoryValues = ProcMeminfoDecoder::getCurrentMemoryValues();
    m_memoryUsedPercent = memoryValues.totalMemory == 0.0 ? 0.0 : float(memoryValues.totalMemory - memoryValues.freeMemory) / float(memoryValues.totalMemory) * 100.0;
}

float TotalMemoryTracker::getMemoryUsedPercent() const
{
    return m_memoryUsedPercent;
}
