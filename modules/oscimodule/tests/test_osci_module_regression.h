#ifndef TEST_OSCI_MODULE_REGRESSION_H
#define TEST_OSCI_MODULE_REGRESSION_H

#include "modulemanagertestrunner.h"

class test_osci_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void dumpDspSetup();
    void dumpDspSetReference();
private:
         // we expect channel m-Name!!!
    void setReferenceChannel(ModuleManagerTestRunner &modman, const QString &channelAlias);
};

#endif // TEST_OSCI_MODULE_REGRESSION_H
