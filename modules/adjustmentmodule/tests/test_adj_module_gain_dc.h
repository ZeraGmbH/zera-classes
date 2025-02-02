#ifndef TEST_ADJ_MODULE_GAIN_DC_H
#define TEST_ADJ_MODULE_GAIN_DC_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QObject>

class test_adj_module_gain_dc : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void noActValuesWithPermission();
    void validActValuesWithPermission();
    void validActValuesWithPermissionNeg();
    void validActValuesWithoutPermission();

    void outOfLimitLower();
    void outOfLimitLowerNeg1();
    void outOfLimitLowerNeg2();
    void outOfLimitUpper();
    void outOfLimitUpperNeg1();
    void outOfLimitUpperNeg2();
    void denyRangeNotSet();
    void withinLimitLowerButAcValues();

    void oneNodeWithinLimitLower();
    void oneNodeWithinLimitLowerNeg1();
    void oneNodeWithinLimitLowerNeg2(); // TODO: What makes our interpolation on range adjustment out of it???
    void oneNodeWithinLimitUpper();
    void oneNodeWithinLimitUpperNeg1();
    void oneNodeWithinLimitUpperNeg2(); // TODO: What makes our interpolation on range adjustment out of it???

private:
    void destroyCommonTestRunner();
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
};

#endif // TEST_ADJ_MODULE_GAIN_DC_H
