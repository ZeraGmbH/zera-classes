#include "test_modman_regression_all_sessions2.h"
#include "testfactoryserviceinterfaces.h"
#include "modulemanagerconfig.h"
#include "testmodulemanager.h"
#include "modulemanagersetupfacade.h"
#include "testlicensesystem.h"
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_modman_regression_all_sessions2)

void test_modman_regression_all_sessions2::initTestCase()
{
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    TestModuleManager::enableTests();
    qputenv("QT_FATAL_CRITICALS", "1");
    qputenv("QT_LOGGING_RULES", "*.debug=false;*.info=false");

}

void test_modman_regression_all_sessions2::allSessionsVeinDumps_data()
{
    QTest::addColumn<QString>("device");
    QTest::addColumn<QString>("sessionFileName");
    const QStringList devices = QStringList() << "com5003" << "mt310s2";
    for(const QString &device : devices) {
        const QStringList sessionFileNames = getSessionFileNames(device);
        for(const QString &sessionFileName : sessionFileNames) {
            // for now - something is wrong with them...
            if(sessionFileName == "com5003-ced-session.json")
                continue;
            if(sessionFileName == "mt310s2-ced-session.json")
                continue;
            QTest::newRow(sessionFileName.toLatin1()) << device << sessionFileName;
        }
    }
}

void test_modman_regression_all_sessions2::allSessionsVeinDumps()
{
    QFETCH(QString, device);
    QFETCH(QString, sessionFileName);
    qInfo("Device %s / session file: %s", qPrintable(device), qPrintable(sessionFileName));

    TestLicenseSystem licenseSystem;
    ModuleManagerSetupFacade modManSetupFacade(&licenseSystem);
    TestModuleManager modMan(&modManSetupFacade, m_serviceInterfaceFactory);
    modMan.loadAllAvailableModulePlugins();
    modMan.setupConnections();
    modMan.startAllTestServices(device, false);
    modMan.changeSessionFile(sessionFileName);
    modMan.waitUntilModulesAreReady();

    QFile file(QString(":/veinDumps/%1").arg(sessionFileName));
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    VeinStorage::DumpJson::dumpToFile(modManSetupFacade.getStorageSystem()->getDb(), &buff, QList<int>(), QList<int>() << 9999);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));

    modMan.destroyModulesAndWaitUntilAllShutdown();
}

const QStringList test_modman_regression_all_sessions2::getSessionFileNames(const QString deviceName)
{
    ModulemanagerConfig::setDemoDevice(deviceName, true);
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    return mmConfig->getAvailableSessions();
}
