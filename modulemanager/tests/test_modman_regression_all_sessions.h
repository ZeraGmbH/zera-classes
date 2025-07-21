#ifndef TEST_MODMAN_REGRESSION_ALL_SESSIONS_H
#define TEST_MODMAN_REGRESSION_ALL_SESSIONS_H

#include "devicesexportgenerator.h"
#include "testmodulemanager.h"
#include <abstractfactoryserviceinterfaces.h>
#include <testopenfiletracker.h>

class test_modman_regression_all_sessions : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void allSessionsVeinDumps_data();
    void allSessionsVeinDumps();
    void uniqueEntityNameEntityIdPairsCom5003();
    void uniqueEntityNameEntityIdPairsMt310s2();
    void uniqueEntityNameEntityIdPairsMt581s2();
    void testGenerateScpiDocs();
    void testGenerateSnapshots();
    void checkObjectsProperlyDeleted();
    void checkFilesProperlyClosed();
private:
    bool checkUniqueEntityIdNames(const QString& device);
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    VeinDumps m_veinDumps;
    QList<TestModuleManager::TModuleInstances> m_instanceCountsOnModulesDestroyed;
    QString m_devIfaceXmlsPath;
    QString m_snapshotJsonsPath;
    std::unique_ptr<TestOpenFileTracker> m_openFileTracker;
};

#endif // TEST_MODMAN_REGRESSION_ALL_SESSIONS_H
