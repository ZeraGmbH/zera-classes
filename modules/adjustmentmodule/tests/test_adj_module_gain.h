#ifndef TEST_ADJ_MODULE_GAIN_H
#define TEST_ADJ_MODULE_GAIN_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_adj_module_gain : public QObject
{
    Q_OBJECT
private slots:
    void noActValuesWithPermission();
    void validActValuesWithPermission();
    void validActValuesWithoutPermission();

    // from here on with permission
    void outOfLimitLower();
    void outOfLimitUpper();
    void withinLimitLower();
    void withinLimitUpper();

private:
    void setActualTestValues(ModuleManagerTestRunner &testRunner);
};

#endif // TEST_ADJ_MODULE_GAIN_H
