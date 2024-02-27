#ifndef TEST_RMS_MODULE_REGRESSION_H
#define TEST_RMS_MODULE_REGRESSION_H

#include "licensesystemmock.h"
#include "modulemanagersetupfacade.h"
#include "testfactoryserviceinterfaces.h"
#include "testmodulemanager.h"
#include <QObject>
#include <memory>

class test_rms_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectActualValues();
    void injectActualTwice();
};

#endif // TEST_RMS_MODULE_REGRESSION_H
