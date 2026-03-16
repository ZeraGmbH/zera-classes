#ifndef DSPCOMMONSUPERVISOR_H
#define DSPCOMMONSUPERVISOR_H

#include <QtGlobal>
#include <memory>

struct DspCommonSupervisorEntry {
    float m_percentBusy = 0.0;
    int m_periodCount = 0;
    int m_msTimerCount = 0;
};

class DspCommonSupervisor
{
public:
    DspCommonSupervisor();
};

typedef std::shared_ptr<DspCommonSupervisor> DspCommonSupervisorPtr;

#endif // DSPCOMMONSUPERVISOR_H
