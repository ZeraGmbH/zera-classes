#ifndef DSPCOMMONSUPERVISOR_H
#define DSPCOMMONSUPERVISOR_H

#include <QtGlobal>
#include <QList>
#include <memory>

struct DspCommonSupervisorEntry {
    float m_percentBusy = 0.0;
    int m_periodCount = 0;
    int m_msTimer = 0;
};

enum DspCommonSupervisorEntryFields {
    PERCENT_BUSY_FIELD = 0,
    PERIOD_COUNT_FIELD,
    MS_TIMER_FIELD,

    COUNT_SUPER_ENTRIES
};

class DspCommonSupervisor
{
public:
    void setSuperList(const QList<DspCommonSupervisorEntry> &superList);
    const QList<DspCommonSupervisorEntry> &getSuperList() const;
private:
    QList<DspCommonSupervisorEntry> m_superList;
};

typedef std::shared_ptr<DspCommonSupervisor> DspCommonSupervisorPtr;

#endif // DSPCOMMONSUPERVISOR_H
