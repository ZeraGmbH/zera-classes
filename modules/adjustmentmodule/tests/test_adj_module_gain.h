#ifndef TEST_ADJ_MODULE_GAIN_H
#define TEST_ADJ_MODULE_GAIN_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QObject>

class test_adj_module_gain : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void noActValuesWithPermission();
    void validActValuesWithPermission();
    void validActValuesWithoutPermission();

    void outOfLimitLower();
    void outOfLimitUpper();
    void denyRangeNotSet();
    void withinLimitLowerButDcValues();

    void oneNodeWithinLimitLower();
    void oneNodeWithinLimitUpper();

    void twoNodesCheckNodesGenerated();

    // Test on correction values should go zenux-services
    void oneNodeCheckCorrectionsCalculated();
    void twoNodesCheckCorrectionsCalculatedLinear();
    void twoNodesCheckCorrectionsCalculatedNonLinear();

private:
    void destroyCommonTestRunner();
    void resetLooksLikeABugOnInit(); // TODO: Do coefficients remain for recalibration?

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
};

#endif // TEST_ADJ_MODULE_GAIN_H
