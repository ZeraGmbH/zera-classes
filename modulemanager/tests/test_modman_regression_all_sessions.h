#ifndef TEST_MODMAN_REGRESSION_ALL_SESSIONS_H
#define TEST_MODMAN_REGRESSION_ALL_SESSIONS_H

#include "devicesexportgenerator.h"
#include <abstractfactoryserviceinterfaces.h>

class test_modman_regression_all_sessions : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void allSessionsVeinDumps_data();
    void allSessionsVeinDumps();
    void uniqueEntityNameEntityIdPairsCom5003();
    void uniqueEntityNameEntityIdPairsMt310s2();
    void testGenerateScpiDocs();
private:
    bool checkUniqueEntityIdNames(const QString& device);
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    VeinDumps m_veinDumps;
    QString m_devIfaceXmlsPath;
};

#endif // TEST_MODMAN_REGRESSION_ALL_SESSIONS_H
