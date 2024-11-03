#include "scpiifaceexportgenerator.h"
#include "factoryserviceinterfaces.h"
#include <QFile>

ScpiIfaceExportGenerator::ScpiIfaceExportGenerator()
{
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    ModulemanagerConfig::setDemoDevice("mt310s2", false);
    m_modmanConfig = ModulemanagerConfig::getInstance();
}

ScpiIfaceExportGenerator::~ScpiIfaceExportGenerator()
{
    destroyModules();
}

void ScpiIfaceExportGenerator::createModman(QString device)
{
    ModulemanagerConfig::setDemoDevice(device, false);
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_modmanSetupFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get(), m_modmanConfig->isDevMode());
    m_modman = std::make_unique<TestModuleManager>(m_modmanSetupFacade.get(), std::make_shared<FactoryServiceInterfaces>());

    m_modman->loadAllAvailableModulePlugins();
    m_modman->setupConnections();
    m_modman->startAllTestServices(device, false);
}

void ScpiIfaceExportGenerator::destroyModules()
{
    if(m_modman)
        m_modman->destroyModulesAndWaitUntilAllShutdown();
}

void ScpiIfaceExportGenerator::setDevice(QString device)
{
    if(m_device != device) {
        destroyModules();
        createModman(device);
        m_device = device;
    }
}

QString ScpiIfaceExportGenerator::getSessionScpiIface(QString device, QString session)
{
    QString scpiIface;
    if(m_modmanConfig->getAvailableSessions().contains(session)) {
        m_modman->changeSessionFile(session);
        m_modman->waitUntilModulesAreReady();
        scpiIface = m_modmanSetupFacade->getStorageSystem()->getDb()->getStoredValue(9999, "ACT_DEV_IFACE").toString();
    }
    return scpiIface;
}

void ScpiIfaceExportGenerator::getAllSessionsScpiIfaceXmls(QString device, QString xmlDir)
{
    QString scpiIface;
    setDevice(device);
    for(const QString &session: m_modmanConfig->getAvailableSessions()) {
        scpiIface = getSessionScpiIface(device, session);
        QString xmlFileName(xmlDir + session);
        xmlFileName.replace("json", "xml");
        createXml(xmlFileName, scpiIface);
    }
}

void ScpiIfaceExportGenerator::createXml(QString completeFileName, QString contents)
{
    QFile xmlFile(completeFileName);
    xmlFile.open(QFile::ReadWrite);
    QTextStream data(&xmlFile);
    data << contents;
}
