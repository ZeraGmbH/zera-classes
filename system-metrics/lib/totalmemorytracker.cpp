#include "totalmemorytracker.h"
#include "procmeminfodecoder.h"

float TotalMemoryTracker::CalculateMemoryUsedPercent()
{
    MemoryValues memoryValues = ProcMeminfoDecoder::getCurrentMemoryValues();
    float memoryUsedPercent = memoryValues.totalMemory == 0.0 ? 0.0 : float(memoryValues.totalMemory - memoryValues.freeMemory) / float(memoryValues.totalMemory) * 100.0;
    return memoryUsedPercent;
}
