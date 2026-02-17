#ifndef TEST_PERIODAVERAGE_MODULE_H
#define TEST_PERIODAVERAGE_MODULE_H

#include <QObject>

class test_periodaverage_module : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void veinDumpInitial();

    void veinDumpInitialAllChannels5PeriodsMax();
    void veinDumpInitialAuxChannels13PeriodsMax();

    void dumpDspSetupAllChannels5PeriodsMax();
    void dumpDspSetupAuxChannels13PeriodsMax();

};

#endif // TEST_PERIODAVERAGE_MODULE_H
