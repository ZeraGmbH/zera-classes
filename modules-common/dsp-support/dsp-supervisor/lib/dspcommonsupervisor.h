#ifndef DSPCOMMONSUPERVISOR_H
#define DSPCOMMONSUPERVISOR_H

#include <QtGlobal>
#include <QList>
#include <QMap>
#include <QDateTime>
#include <memory>

// This is about
// * DSP processing load
// * DSP period timestamps
//
// Maybe the term 'supervisor' is not the best but we were looking for something
// starting with 'super' :)

struct DspSupervisorInput {
    float m_percentBusy = 0.0;
    quint32 m_periodCount = 0;
    quint32 m_msTimer = 0;
};

enum DspSupervisorInputFields {
    PERCENT_BUSY_FIELD = 0,
    PERIOD_COUNT_FIELD,
    MS_TIMER_FIELD,

    COUNT_SUPER_ENTRIES
};

struct DspSupervisorOutput {
    DspSupervisorInput m_rawIn;
    QDateTime m_timeStamp;
};

typedef QMap<quint32, DspSupervisorOutput> DspSupervisorOutputMap;

class DspCommonSupervisor
{
public:
    void addSupervisorEntries(const QList<DspSupervisorInput>& entriesChronological, int maxEntries);
    const DspSupervisorOutputMap &getSupervisorMap() const;
    quint32 getCurrentPeriod() const;
    quint32 getCurrentMsTime() const;
private:
    DspSupervisorOutputMap m_supervisorMap;
    QList<quint32> m_entryStore;
    quint32 m_currentPeriod = 0;
    quint32 m_currentMsTime = 0;
};

typedef std::shared_ptr<DspCommonSupervisor> DspCommonSupervisorPtr;

#endif // DSPCOMMONSUPERVISOR_H
