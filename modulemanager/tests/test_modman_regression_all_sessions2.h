#ifndef TEST_MODMAN_REGRESSION_ALL_SESSIONS2_H
#define TEST_MODMAN_REGRESSION_ALL_SESSIONS2_H

#include <abstractfactoryserviceinterfaces.h>

class test_modman_regression_all_sessions2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void allSessionsVeinDumps_data();
    void allSessionsVeinDumps();
private:
    const QStringList getSessionFileNames(const QString deviceName);
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
};

#endif // TEST_MODMAN_REGRESSION_ALL_SESSIONS2_H
