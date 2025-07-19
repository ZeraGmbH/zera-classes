#include "sessionexportgenerator.h"
#include "factoryserviceinterfaces.h"
#include "vf_client_component_setter.h"
#include <timemachineobject.h>
#include <jsonloggercontentloader.h>
#include <jsonloggercontentsessionloader.h>
#include <loggercontentsetconfig.h>
#include <vl_sqlitedb.h>
#include <vs_dumpjson.h>
#include <testsqlitedb.h>
#include <QFile>
#include <QDir>
#include <memory>

constexpr int system_entity = 0;
constexpr int scpi_module_entity = 9999;
constexpr int vf_logger_entity = 2;

SessionExportGenerator::SessionExportGenerator(const LxdmSessionChangeParam &lxdmParam) :
    m_lxdmParam(lxdmParam)
{
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    ModulemanagerConfig::setDemoDevice("mt310s2");
    m_modmanConfig = ModulemanagerConfig::getInstance();
}

SessionExportGenerator::~SessionExportGenerator()
{
    destroyModules();
}

void SessionExportGenerator::createModman(QString device)
{
    qInfo("Create modman for device: %s\n", qPrintable(device));
    ModulemanagerConfig::setDemoDevice(device);
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_modmanSetupFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get(),
                                                                     m_modmanConfig->isDevMode(),
                                                                     m_lxdmParam,
                                                                     "/tmp/" + QUuid::createUuid().toString(QUuid::WithoutBraces));
    m_modman = std::make_unique<TestModuleManager>(m_modmanSetupFacade.get(), std::make_shared<FactoryServiceInterfaces>());

    m_testSignaller = std::make_unique<TestDbAddSignaller>();
    const VeinLogger::DBFactory sqliteFactory = [=]() -> std::shared_ptr<VeinLogger::AbstractLoggerDB> {
        return std::make_shared<TestSQLiteDB>(m_testSignaller.get());
    };
    m_dataLoggerSystem = std::make_unique<VeinLogger::DatabaseLogger>(
        m_modmanSetupFacade->getStorageSystem(),
        sqliteFactory,
        this,
        QList<int>()
            << 0    /* SYSTEM */
            << 1150 /* STATUS */);;

    //setup logger
    QString OE_MODMAN_CONTENTSET_PATH = QDir::cleanPath(QString(OE_INSTALL_ROOT) + "/" + QString(MODMAN_CONTENTSET_PATH));
    QString OE_MODMAN_SESSION_PATH = QDir::cleanPath(QString(OE_INSTALL_ROOT) + "/" + QString(MODMAN_SESSION_PATH));
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(OE_MODMAN_CONTENTSET_PATH, std::make_shared<JsonLoggerContentLoader>());
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(OE_MODMAN_SESSION_PATH, std::make_shared<JsonLoggerContentSessionLoader>());

    m_dataLoggerSystemInitialized = false;
    connect(m_modmanSetupFacade->getLicenseSystem(), &LicenseSystemInterface::sigSerialNumberInitialized, [&](){
        if(m_licenseSystem->isSystemLicensed(m_dataLoggerSystem->entityName()))
        {
            if(!m_dataLoggerSystemInitialized)
            {
                m_dataLoggerSystemInitialized = true;
                qInfo("Starting DataLoggerSystem...");
                m_modmanSetupFacade->addSubsystem(m_dataLoggerSystem.get());
            }
        }
    });

    m_modman->loadAllAvailableModulePlugins();
    m_modman->setupConnections();
    m_modman->startAllTestServices(device, false);
    m_modmanSetupFacade->getLicenseSystem()->setDeviceSerial("foo");
    TimeMachineObject::feedEventLoop();
}

void SessionExportGenerator::destroyModules()
{
    if(m_modman)
        m_modman->destroyModulesAndWaitUntilAllShutdown();
}

void SessionExportGenerator::setDevice(QString device)
{
    if(m_device != device) {
        destroyModules();
        createModman(device);
        m_device = device;
    }
}

QStringList SessionExportGenerator::getAvailableSessions()
{
    return m_modmanConfig->getAvailableSessions();
}

void SessionExportGenerator::changeSession(QString session)
{
    m_modman->changeSessionFile(session);
    m_modman->waitUntilModulesAreReady();
}

void SessionExportGenerator::generateDevIfaceXml(QString xmlDir)
{
    QString scpiIface = getVfComponent(scpi_module_entity, "ACT_DEV_IFACE").toString();
    QString currentSession = getVfComponent(system_entity, "Session").toString();
    QString xmlFileName(xmlDir + currentSession);
    xmlFileName.replace("json", "xml");
    createXml(xmlFileName, scpiIface);
}

void SessionExportGenerator::generateSnapshotJsons(QString snapshotDir)
{
    QString currentSession = getVfComponent(system_entity, "Session").toString();
    QStringList availableContentSets = getVfComponent(vf_logger_entity, "availableContentSets").toStringList();
    for(QString contentSet: availableContentSets) {
        QString snapshotFileName = currentSession.replace(".json", "") + '-' + contentSet + ".json";
        QFile snapshotFile(snapshotDir + snapshotFileName);
        snapshotFile.open(QFile::ReadWrite);
    }
}

QByteArray SessionExportGenerator::getVeinDump()
{
    return VeinStorage::DumpJson::dumpToByteArray(m_modmanSetupFacade->getStorageSystem()->getDb(), QList<int>(), QList<int>() << scpi_module_entity);
}

QList<TestModuleManager::TModuleInstances> SessionExportGenerator::getInstanceCountsOnModulesDestroyed()
{
    return m_modman->getInstanceCountsOnModulesDestroyed();
}

void SessionExportGenerator::createXml(QString completeFileName, QString contents)
{
    QFile xmlFile(completeFileName);
    xmlFile.open(QFile::ReadWrite);
    QTextStream data(&xmlFile);
    data << contents;
}

void SessionExportGenerator::setVfComponent(int entityId, QString componentName, QVariant newValue)
{
    QVariant oldValue = m_modmanSetupFacade->getStorageSystem()->getDb()->getStoredValue(entityId, componentName);
    QEvent* event = VfClientComponentSetter::generateEvent(entityId, componentName, oldValue, newValue);
    emit m_modmanSetupFacade->getStorageSystem()->sigSendEvent(event);
    TimeMachineObject::feedEventLoop();
}

QVariant SessionExportGenerator::getVfComponent(int entityId, QString componentName)
{
    return m_modmanSetupFacade->getStorageSystem()->getDb()->getStoredValue(entityId, componentName);
}
