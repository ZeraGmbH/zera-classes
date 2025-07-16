#include "sessionexportgenerator.h"
#include "factoryserviceinterfaces.h"
#include <vs_dumpjson.h>
#include <QFile>

constexpr int system_entity = 0;
constexpr int scpi_module_entity = 9999;

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

    m_modman->loadAllAvailableModulePlugins();
    m_modman->setupConnections();
    m_modman->startAllTestServices(device, false);
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
    QString scpiIface = m_modmanSetupFacade->getStorageSystem()->getDb()->getStoredValue(scpi_module_entity, "ACT_DEV_IFACE").toString();
    QString currentSession = m_modmanSetupFacade->getStorageSystem()->getDb()->getStoredValue(system_entity, "Session").toString();
    QString xmlFileName(xmlDir + currentSession);
    xmlFileName.replace("json", "xml");
    createXml(xmlFileName, scpiIface);
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
