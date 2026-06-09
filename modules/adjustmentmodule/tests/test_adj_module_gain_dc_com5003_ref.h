#ifndef TEST_ADJ_MODULE_GAIN_DC_COM5__3_REF_H
#define TEST_ADJ_MODULE_GAIN_DC_COM5__3_REF_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QObject>

class test_adj_module_gain_dc_com5003_ref : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void initTestCase_data();
    void init();
    void cleanup();

    void checkIntialRangeR10V();

    void initR0VOk(); // init allowed for not adjustable ranges
    void initR10VOk();

    void offsetStatusSetR0VOk(); // status set allowed for not adjustable ranges
    void offsetStatusSetR10VOk();

    void gainStatusSetR0VOk(); // status set allowed for not adjustable ranges
    void gainStatusSetR10VOk();

    void phaseStatusSetR0VOk(); // status set allowed for not adjustable ranges
    void phaseStatusSetR10VOk();

    void offsetAdjR0VRejected();
    void offsetAdjR10VRejected(); // COM5003 adjusts offset automatically at start of ref session

    void acGainAdjR0VRejected();
    void acGainAdjR10VRejected();

    void dcgainAdjR0VRejected();
    void dcgainAdjR10VOk();

    void phaseAdjR0VRejected();
    void phaseAdjR10VRejected();

    // from here fresh client / server
    void checkCalculatedAdjValuesPerChannel();
    void checkCalculatedAdjValuesAccumulatedSequence();

private:
    QString setAutoRangeOff();
    QString setRangeGroupingOff();
    QString setRange(const QString &channelAlias, const QString &rangeName);
    void initClientServer();

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;

    QString m_refChannel;
};

#endif // TEST_ADJ_MODULE_GAIN_DC_COM5__3_REF_H
