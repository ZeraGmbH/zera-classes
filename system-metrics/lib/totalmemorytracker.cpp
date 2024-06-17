#include "totalmemorytracker.h"
#include "procmeminfodecoder.h"
#include <math.h>

void MemoryUsageParams::setZeros()
{
    m_RAMUsedPercent = 0.0;
    m_cachesUsedPercent = 0.0;
    m_buffersUsedPercent = 0.0;
}

void TotalMemoryTracker::calculateMemoryUsedPercent()
{
    MemoryValues memoryValues = ProcMeminfoDecoder::getCurrentMemoryValues();
    quint32 usedMemory = memoryValues.totalMemory - memoryValues.freeMemory;
    quint32 buffersAndCachedMemory = memoryValues.buffers + memoryValues.cached;
    quint32 usedRAM = usedMemory - buffersAndCachedMemory;
    if(memoryValues.totalMemory != 0.0) {
        m_memoryUsageParams.m_RAMUsedPercent = calcPercentageOneDecimal(float(usedRAM) / float(memoryValues.totalMemory));
        m_memoryUsageParams.m_buffersUsedPercent = calcPercentageOneDecimal(float(memoryValues.buffers) / float(memoryValues.totalMemory));
        m_memoryUsageParams.m_cachesUsedPercent = calcPercentageOneDecimal(float(memoryValues.cached) / float(memoryValues.totalMemory));
    }
    else
        m_memoryUsageParams.setZeros();

}

MemoryUsageParams TotalMemoryTracker::getMemoryUsageParams() const
{
    return m_memoryUsageParams;
}

void TotalMemoryTracker::periodicLogs()
{
    QString logString = QString("Memory used: %1%, Buffers: %2% Cache: %3%")
                            .arg(m_memoryUsageParams.m_RAMUsedPercent, 0, 'f', 1)
                            .arg(m_memoryUsageParams.m_buffersUsedPercent, 0, 'f', 1)
                            .arg(m_memoryUsageParams.m_cachesUsedPercent, 0, 'f', 1);
    if(logString != m_lastLogString) {
        m_lastLogString = logString;
        qInfo("%s", qPrintable(logString));
    }
}

float TotalMemoryTracker::calcPercentageOneDecimal(float value)
{
    float percentageVal = value * 100;
    return round(percentageVal * 10) / 10;
}

