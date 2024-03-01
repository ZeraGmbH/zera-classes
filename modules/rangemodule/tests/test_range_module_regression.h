#ifndef TEST_RANGE_MODULE_REGRESSION_H
#define TEST_RANGE_MODULE_REGRESSION_H

#include "vf_cmd_event_handler_system.h"
#include <QObject>

class test_range_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectActualValues();
    void injectActualValuesWithPreScaling();
    void injectActualValuesWithCheating();
private:
    void setVfPreScaling(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem, QVariant value);
    void setVfIgnoreRmsValues(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem, QVariant value);
};

#endif // TEST_RANGE_MODULE_REGRESSION_H
