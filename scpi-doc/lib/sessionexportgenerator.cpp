#include "sessionexportgenerator.h"
#include "factoryserviceinterfaces.h"
#include <QFile>

SessionExportGenerator::SessionExportGenerator()
{
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    ModulemanagerConfig::setDemoDevice("mt310s2", false);
    m_modmanConfig = ModulemanagerConfig::getInstance();
}

SessionExportGenerator::~SessionExportGenerator()
{
    destroyModules();
}

void SessionExportGenerator::createModman(QString device)
{
    qInfo("Create modman for device: %s\n", qPrintable(device));
    ModulemanagerConfig::setDemoDevice(device, false);
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_modmanSetupFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get(), m_modmanConfig->isDevMode());
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
    QString scpiIface = m_modmanSetupFacade->getStorageSystem()->getDb()->getStoredValue(9999, "ACT_DEV_IFACE").toString();
    QString currentSession = m_modmanSetupFacade->getStorageSystem()->getDb()->getStoredValue(0, "Session").toString();
    QString xmlFileName(xmlDir + currentSession);
    xmlFileName.replace("json", "xml");
    createXml(xmlFileName, scpiIface);
}

void SessionExportGenerator::getAllSessionsScpiIfaceXmls(QString device, QString xmlDir)
{
    QString scpiIface;
    setDevice(device);
    for(const QString &session: m_modmanConfig->getAvailableSessions()) {
        changeSession(session);
        generateDevIfaceXml(xmlDir);
    }
}

void SessionExportGenerator::createXml(QString completeFileName, QString contents)
{
    QFile xmlFile(completeFileName);
    xmlFile.open(QFile::ReadWrite);
    QTextStream data(&xmlFile);
    data << contents;
}
