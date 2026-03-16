#include "dspcommonsupervisor.h"

void DspCommonSupervisor::setSuperList(const QList<DspCommonSupervisorEntry> &superList)
{
    m_superList = superList;
}

const QList<DspCommonSupervisorEntry> &DspCommonSupervisor::getSuperList() const
{
    return m_superList;
}
