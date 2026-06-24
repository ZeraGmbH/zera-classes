#ifndef TEST_SCPI_MODULE_SORTED_RESPONSES_H
#define TEST_SCPI_MODULE_SORTED_RESPONSES_H

#include "modulemanagertestrunner.h"
#include <memory>

class test_scpi_module_sorted_responses : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    // inspired by https://zera.app.remberg.de/de/s1/tickets/detail/6a32d1641deff4af518140c3
    void paramQuerySequence();
    void paramQuerySequenceWithIntermediateServerNotification();
private:
    void spawnServerNotififcation();
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_MODULE_SORTED_RESPONSES_H
