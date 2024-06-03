#ifndef CPUTIMINGVALUES_H
#define CPUTIMINGVALUES_H

#include <QtGlobal>

struct CpuTimingValues
{
    quint64 m_userTime = 0;
    quint64 m_niceTime = 0;
    quint64 m_systemTime = 0;
    quint64 m_idleTime = 0;
    quint64 m_iowaitTime = 0;
    quint64 m_irqTime = 0;
    quint64 m_softIrqTime = 0;
};

#endif // CPUTIMINGVALUES_H
