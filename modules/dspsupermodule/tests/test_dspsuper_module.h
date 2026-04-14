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

    void supervisorMapInitial();
    void supervisorMapOneInterrupt();
    void supervisorMapOneInterruptTwoEntries();
    void supervisorMapTwoInterrupts();
    void supervisorMapThreeInterruptsTimerb32BitTurnaround();
    void supervisorMap10EntriesTotal();
};

#endif // TEST_DSPSUPER_MODULE_H
