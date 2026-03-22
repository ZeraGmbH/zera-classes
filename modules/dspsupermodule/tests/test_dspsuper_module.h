#ifndef TEST_DSPSUPER_MODULE_H
#define TEST_DSPSUPER_MODULE_H

#include <QObject>

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
};

#endif // TEST_DSPSUPER_MODULE_H
