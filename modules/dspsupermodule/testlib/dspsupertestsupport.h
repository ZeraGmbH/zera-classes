#ifndef DSPSUPERTESTSUPPORT_H
#define DSPSUPERTESTSUPPORT_H

#include "dspcommonsupervisor.h"
#include <testdspinterface.h>

class DspSuperTestSupport
{
public:
    static void fireInterrupt(const DspSupervisorInput &superEntry, TestDspInterfacePtr dspInterface);
    static void fireInterrupts(const QList<DspSupervisorInput> &superEntries, TestDspInterfacePtr dspInterface);
};

#endif // DSPSUPERTESTSUPPORT_H
