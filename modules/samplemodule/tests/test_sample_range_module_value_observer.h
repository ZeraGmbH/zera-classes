#ifndef TEST_SAMPLE_RANGE_MODULE_VALUE_OBSERVER_H
#define TEST_SAMPLE_RANGE_MODULE_VALUE_OBSERVER_H

#include "modulemanagertestrunner.h"
#include <QMap>

class test_sample_range_module_value_observer : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void fireSignalOnNewValuesTailoredRangeModule();
    void fireSignalOnNewValuesRealRangeModule();
    void fireSignalTwiceOnSameValuesRealRangeModule();

    void fireAndGetValidValuesMt();
    void fireAndGetInValidValuesMt();

private:
    void createModule(int entityId, QMap<QString, QVariant> components);
    void fireRangeValues(float rmsValue, int phaseCount);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SAMPLE_RANGE_MODULE_VALUE_OBSERVER_H
