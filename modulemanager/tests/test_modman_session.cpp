#include "test_modman_session.h"
#include "modulemanagerconfigtest.h"
#include <QTest>

QTEST_MAIN(test_modman_session)


void test_modman_session::loadSession()
{
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("demo");

}
