#ifndef TEST_ADJ_MODULE_OFFSET_H
#define TEST_ADJ_MODULE_OFFSET_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QObject>

class test_adj_module_offset : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void validActValuesWithPermission();
    void validActValuesWithoutPermission();

    // from here on with permission
    void denyNonOffsetChannel();
    void denyRangeNotSet();

private:
    void destroyCommonTestRunner();

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
};

#endif // TEST_ADJ_MODULE_OFFSET_H
