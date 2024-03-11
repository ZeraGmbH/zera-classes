#ifndef TEST_ADJ_MODULE_PHASE_H
#define TEST_ADJ_MODULE_PHASE_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_adj_module_phase : public QObject
{
    Q_OBJECT
private slots:
    void noActValuesWithPermission();
    void validActValuesWithPermission();
    void validActValuesWithoutPermission();

    // from here on with permission
    void absOutOfRangeLower();
    void absOutOfRangeUpper();
private:
    void setActualTestValues(ModuleManagerTestRunner &testRunner);
};

#endif // TEST_ADJ_MODULE_PHASE_H
