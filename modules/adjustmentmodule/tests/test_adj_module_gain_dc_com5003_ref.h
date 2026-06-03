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

    void init480VRejected();
    void initR0VOk(); // init allowed for not adjustable ranges
    void initR10VOk();

    void offsetStatusSet480VRejected();
    void offsetStatusSetR0VOk(); // status set allowed for not adjustable ranges
    void offsetStatusSetR10VOk();

    void gainStatusSet480VRejected();
    void gainStatusSetR0VOk(); // status set allowed for not adjustable ranges
    void gainStatusSetR10VOk();

    void phaseStatusSet480VRejected();
    void phaseStatusSetR0VOk(); // status set allowed for not adjustable ranges
    void phaseStatusSetR10VOk();

    void offsetAdj480VRejected(); // COM5003 adjusts offset automatically at start of ref session
    void offsetAdjR0VRejected();
    void offsetAdjR10VRejected();

    void acGainAdj480VRejected();
    void acGainAdjR0VRejected();
    void acGainAdjR10VRejected();

    void dcgainAdj480VRejected();
    void dcgainAdjR0VRejected();
    void dcgainAdjR10VOk();

    void phaseAdj480VRejected();
    void phaseAdjR0VRejected();
    void phaseAdjR10VRejected();

private:
    QString setAutoRangeOff();
    QString setRangeGroupingOff();
    QString setRange(const QString &channelAlias, const QString &rangeName);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
    QString m_refChannel;
};

#endif // TEST_ADJ_MODULE_GAIN_DC_COM5__3_REF_H
