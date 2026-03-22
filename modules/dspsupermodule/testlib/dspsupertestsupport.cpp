#include "dspsupertestsupport.h"
#include <timemachineobject.h>

void DspSuperTestSupport::fireInterrupt(const DspSupervisorInput &superEntry, TestDspInterfacePtr dspInterface)
{
    QList<DspSupervisorInput> entries = QList<DspSupervisorInput>() << superEntry;
    fireInterrupts(entries, dspInterface);
}

void DspSuperTestSupport::fireInterrupts(const QList<DspSupervisorInput> &superEntries, TestDspInterfacePtr dspInterface)
{
    QVector<float> rawData;
    for (const DspSupervisorInput &entry : superEntries) {
        rawData.append(entry.m_percentBusy);
        quint32 periodCount32 = entry.m_periodCount;
        rawData.append(*reinterpret_cast<float*>(&periodCount32));
        quint32 msTimer32 = entry.m_msTimer;
        rawData.append(*reinterpret_cast<float*>(&msTimer32));
    }
    dspInterface->fireActValInterrupt(rawData, superEntries.count());
    TimeMachineObject::feedEventLoop();
}
