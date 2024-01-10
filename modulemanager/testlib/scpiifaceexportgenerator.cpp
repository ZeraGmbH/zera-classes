#include "scpiifaceexportgenerator.h"
#include <timemachineobject.h>
#include <QFile>

ScpiIfaceExportGenerator::ScpiIfaceExportGenerator()
{
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    ModulemanagerConfig::setDemoDevice("mt310s2", false);
    m_modmanConfig = ModulemanagerConfig::getInstance();
    m_modmanSetupFacade = new ModuleManagerSetupFacade(&m_licenseSystem, m_modmanConfig->isDevMode());
    m_modman = new ZeraModules::ModuleManager(m_modmanSetupFacade, true);
    m_modman->loadAllAvailableModulePlugins();
    m_modman->setupConnections();
}

ScpiIfaceExportGenerator::~ScpiIfaceExportGenerator()
{
    shutdownModules();
    delete m_modman;
    delete m_modmanSetupFacade;
}

void ScpiIfaceExportGenerator::setDevice(QString device)
{
    if(m_device != device) {
        shutdownModules();
        ModulemanagerConfig::setDemoDevice(device, false);
        m_modman->setMockServices(device);
        m_device = device;
    }
}

void ScpiIfaceExportGenerator::shutdownModules()
{
    m_modman->destroyModules();
    do
        TimeMachineObject::feedEventLoop();
    while(!m_modman->modulesReady());
}

QString ScpiIfaceExportGenerator::getSessionScpiIface(QString device, QString session)
{
    QString scpiIface;
    setDevice(device);
    if(m_modmanConfig->getAvailableSessions().contains(session)) {
        m_modman->changeSessionFile(session);
        do
            TimeMachineObject::feedEventLoop();
        while(!m_modman->modulesReady());
        scpiIface = m_modmanSetupFacade->getStorageSystem()->getStoredValue(9999, "ACT_DEV_IFACE").toString();
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
