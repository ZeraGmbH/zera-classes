#ifndef TEST_DSPSUPER_MODULE_H
#define TEST_DSPSUPER_MODULE_H

#include "dspcommonsupervisor.h"
#include <QObject>
#include <testdspinterface.h>

class test_dspsuper_module : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();

    void minimalSession();
    void dspDumpInitial();
    void zdspDumpInitial();
    void veinDumpInitial();

    void initialSupervisorMap();

    void oneInterrupt();
    void oneInterruptTwoEntries();
    void twoInterrupts();
    void threeInterruptsTimerb32BitTurnaround();

    void max10Entries();
    void veinComponents();

private:
    void fireInterrupt(const DspSupervisorInput &superEntry, TestDspInterfacePtr dspInterface);
    void fireInterrupts(const QList<DspSupervisorInput> &superEntries, TestDspInterfacePtr dspInterface);
};

#endif // TEST_DSPSUPER_MODULE_H
